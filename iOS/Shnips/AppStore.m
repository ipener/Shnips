// Copyright © 2018 Igor Pener. All rights reserved.

#import "AppStore.h"

#import <StoreKit/StoreKit.h>
#import <UIKit/UIKit.h>

#define kInAppPurchaseId(x) @("ch.techos.shnips." #x)

@interface AppStore () <SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    UIActivityIndicatorView *_indicatorView;
    NSArray<SKProduct *>    *_products;
    SKProductsRequest       *_productRequest;
    id<AppStoreDelegate>     _delegate;
}

@end

@implementation AppStore

- (instancetype)initWithDelegate:(id<AppStoreDelegate>)delegate {
    if (self = [super init]) {
        _delegate = delegate;
        _indicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
        _indicatorView.frame = [UIScreen mainScreen].bounds;
        _indicatorView.backgroundColor = [UIColor colorWithWhite:0.f alpha:0.2f];
        _indicatorView.hidden = YES;
        _indicatorView.userInteractionEnabled = YES;

        [SKPaymentQueue.defaultQueue addTransactionObserver:self];
        NSSet<NSString *> *productIds = [NSSet setWithArray:@[kInAppPurchaseId(in_app_purchase_type_hearts)]];
        _productRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIds];
        _productRequest.delegate = self;
        [_productRequest start];
    }
    return self;
}

- (void)startPurchase:(in_app_purchase_type)type {
    if (!_products || _products.count == 0) {
        [self showAlert:NSLocalizedString(@"cannot_connect", nil)];
        return [_delegate appStoreDidCancelInAppPurchase];
    }
    if (SKPaymentQueue.canMakePayments) {
        NSString *productId;
        if (type == in_app_purchase_type_hearts) {
            productId = kInAppPurchaseId(in_app_purchase_type_hearts);
        }
        for (SKProduct *product in _products) {
            if ([product.productIdentifier isEqualToString:productId]) {
                if (_indicatorView.superview == nil) {
                    [UIApplication.sharedApplication.keyWindow.rootViewController.view addSubview:_indicatorView];
                }
                _indicatorView.hidden = NO;
                [_indicatorView startAnimating];
                return [SKPaymentQueue.defaultQueue addPayment:[SKPayment paymentWithProduct:product]];
            }
        }
    } else {
        [self showAlert:NSLocalizedString(@"cannot_make_payments", nil)];
    }
    [_delegate appStoreDidCancelInAppPurchase];
}

- (void)restorePurchases {
    [SKPaymentQueue.defaultQueue restoreCompletedTransactions];
}

// MARK: - SKProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    _productRequest = nil;
    if (response.products && response.products.count) {
        _products = response.products;
        for (SKProduct *product in _products) {
            NSString *productId = product.productIdentifier;
            NSNumberFormatter *numberFormatter = [NSNumberFormatter new];
            [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
            [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
            [numberFormatter setLocale:product.priceLocale];
            NSString *price = [numberFormatter stringFromNumber:product.price];
            if ([productId isEqualToString:kInAppPurchaseId(in_app_purchase_type_hearts)]) {
                [_delegate appStoreDidRetrieveInAppPurchase:in_app_purchase_type_hearts withPrice:price];
            } else {
                NSString *log = [NSString stringWithFormat:@"Unexpected product retrieved: %@", product];
                [_delegate appStoreDidFailWithError:log];
                continue;
            }
        }
    } else {
        NSString *log = [NSString stringWithFormat:@"productsRequest: %@ didReceiveResponse: %@", request, response];
        [_delegate appStoreDidFailWithError:log];
    }
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    _productRequest = nil;
    [_delegate appStoreDidFailWithError:[NSString stringWithFormat:@"request: %@ didFailWithError: %@", request, error]];
}

// MARK: - SKPaymentTransactionObserver

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased:
                return [self completeTransaction:transaction];
            case SKPaymentTransactionStateFailed:
                return [self failedTransaction:transaction];
            case SKPaymentTransactionStateRestored:
                return [self restoreTransaction:transaction];
            case SKPaymentTransactionStateDeferred:
            case SKPaymentTransactionStatePurchasing:
            default:
                return;
        }
    }
}

// MARK: -

- (void)showAlert:(NSString *)description {
    __weak UIViewController *weakViewController = UIApplication.sharedApplication.keyWindow.rootViewController;

    UIAlertController *alert = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"purchase_cancelled", nil)
                                                                   message:description
                                                            preferredStyle:UIAlertControllerStyleAlert];
    [alert addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"ok", nil)
                                              style:UIAlertActionStyleDefault
                                            handler:^(UIAlertAction * action) {
                                                __strong UIViewController *strongViewController = weakViewController;
                                                [strongViewController dismissViewControllerAnimated:YES completion:nil];
                                            }]];
    [weakViewController presentViewController:alert animated:YES completion:nil];
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
    [SKPaymentQueue.defaultQueue finishTransaction:transaction];
    [self provideProduct:transaction.payment.productIdentifier];
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    NSString *description = transaction.error.localizedDescription;
    NSString *log = [NSString stringWithFormat:@"failedTransaction: %@ with error: %@", transaction, description];
    [_delegate appStoreDidFailWithError:log];
    [self showAlert:description];
    [_delegate appStoreDidCancelInAppPurchase];
    if (!_indicatorView.hidden) {
        [_indicatorView stopAnimating];
        _indicatorView.hidden = YES;
    }
    [SKPaymentQueue.defaultQueue finishTransaction:transaction];
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
    [self provideProduct:transaction.originalTransaction.payment.productIdentifier];
    [SKPaymentQueue.defaultQueue finishTransaction:transaction];
}

- (void)provideProduct:(NSString *)productId {
    in_app_purchase_type type;

    if ([productId isEqualToString:kInAppPurchaseId(in_app_purchase_type_hearts)]) {
        type = in_app_purchase_type_hearts;
    } else {
        type = in_app_purchase_type_range;
    }
    if (type < in_app_purchase_type_range) {
        [_delegate appStoreDidBuyInAppPurchase:type];
    } else {
        NSString *log = [NSString stringWithFormat:@"Unexpected product encountered: %@", productId];
        [_delegate appStoreDidFailWithError:log];
    }
    if (!_indicatorView.hidden) {
        [_indicatorView stopAnimating];
        _indicatorView.hidden = YES;
    }
}

@end
