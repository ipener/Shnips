// Copyright © 2018 Igor Pener. All rights reserved.

#import <Foundation/Foundation.h>

#import "in_app_purchase_type.h"

@protocol AppStoreDelegate<NSObject>

- (void)appStoreDidRetrieveInAppPurchase:(in_app_purchase_type)type withPrice:(NSString *)price;

- (void)appStoreDidFailWithError:(NSString *)log;

- (void)appStoreDidBuyInAppPurchase:(in_app_purchase_type)type;

- (void)appStoreDidCancelInAppPurchase;

@end

@interface AppStore : NSObject

- (instancetype)initWithDelegate:(id<AppStoreDelegate>)delegate;

- (void)startPurchase:(in_app_purchase_type)type;

- (void)restorePurchases;

@end
