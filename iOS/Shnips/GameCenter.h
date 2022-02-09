// Copyright © 2018 Igor Pener. All rights reserved.

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@protocol GameCenterDelegate<NSObject>

- (void)gameCenterDidFailWithError:(NSString *)log;

- (void)gameCenterDidRetrieveRank:(NSInteger)rank forTimeScope:(GKLeaderboardTimeScope)timeScope;

- (void)gameCenterDidAuthenticate:(BOOL)authenticated;

@end

@interface GameCenter : NSObject

- (instancetype)initWithDelegate:(id<GameCenterDelegate>)delegate;

- (void)showAchievements;

- (void)showLeaderboard;

- (void)loadLeaderboardData;

// progress is assumed to be of length achievement_type_range
- (void)setAchievementsProgress:(const float *)progress;

- (void)reportScore:(int64_t)score;

- (BOOL)authenticated;

@end
