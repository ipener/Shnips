// Copyright © 2018 Igor Pener. All rights reserved.

#import "ReachabilityController.h"

#import "Reachability.h"

@interface ReachabilityController () {
    Reachability *_internetReachable;
    Reachability *_hostReachable;

    void(^_action)(BOOL, BOOL);
}

@end

@implementation ReachabilityController

- (instancetype)initWithURL:(NSString *)url andAction:(void(^)(BOOL, BOOL))action {
    if (self = [super init]) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(didReceiveReachabilityChangedNotification:)
                                                     name:kReachabilityChangedNotification
                                                   object:nil];
        _action = action;
        _internetReachable = [Reachability reachabilityForInternetConnection];
        _hostReachable = [Reachability reachabilityWithHostName:url];

        [_internetReachable startNotifier];
        [_hostReachable startNotifier];
    }

    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

// MARK: -

-(void)didReceiveReachabilityChangedNotification:(NSNotification *)notification {
    _action([self currentHostReachabilityStatus], [self currentInternetReachabilityStatus]);
}

- (BOOL)currentHostReachabilityStatus {
    switch ([_hostReachable currentReachabilityStatus]) {
        case NotReachable:
            NSLog(@"A gateway to the host server is down.");
            return NO;
        case ReachableViaWiFi:
            NSLog(@"A gateway to the host server is working via WIFI.");
            return YES;
        case ReachableViaWWAN:
            NSLog(@"A gateway to the host server is working via WWAN.");
            return YES;
    }
}

- (BOOL)currentInternetReachabilityStatus {
    switch ([_internetReachable currentReachabilityStatus]) {
        case NotReachable:
            NSLog(@"The internet is down.");
            return NO;
        case ReachableViaWiFi:
            NSLog(@"The internet is working via WIFI.");
            return YES;
        case ReachableViaWWAN:
            NSLog(@"The internet is working via WWAN.");
            return YES;
    }
}

@end
