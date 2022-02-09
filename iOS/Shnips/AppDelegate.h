// Copyright © 2018 Igor Pener. All rights reserved.

#import <UIKit/UIKit.h>

#include "in_app_purchase_type.h"

@interface AppDelegate : UIResponder<UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

- (void)setPaused:(BOOL)paused;

- (void)setAchievementsProgress:(const float *)progress;

- (void)setLeaderboardData:(unsigned long long)score;

- (void)getLeaderboardData;

- (void)showAppRatingView;

- (void)showTwitterWebsite;

- (void)showAchievements;

- (void)showLeaderboard;

- (BOOL)authenticatedForLeaderboard;

- (void)startPurchase:(in_app_purchase_type)type;

- (void)share:(const char *)str;

- (void)scheduleNotifications:(const char **)titles;

- (void)cancelNotifications;

- (void)restorePurchases;

- (void)promptForNotificationPermission;

@end
