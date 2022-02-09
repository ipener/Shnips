// Copyright © 2018 Igor Pener. All rights reserved.

#import "GameCenter.h"

#include "achievement_type.h"

#define kLeaderboardId @"ch.techos.shnips.leaderboard_type_highscore"

@interface GameCenter () <GKGameCenterControllerDelegate> {
    UIViewController      *_authenticateViewController;
    id<GameCenterDelegate> _delegate;
    int64_t                _score;
}

@end

@implementation GameCenter

NSString *kAchievementIds[achievement_type_range] = {
    @"achievement_type_tutorial_completed",
    @"achievement_type_successful_shots",
    @"achievement_type_cushion_shots",
    @"achievement_type_no_cushion_shots",
    @"achievement_type_consecutive_shots",
    @"achievement_type_consecutive_fast_shots",
    @"achievement_type_chip_type_fast_unlocked",
    @"achievement_type_chip_type_extra_points_unlocked",
    @"achievement_type_chip_type_swap_unlocked",
    @"achievement_type_chip_type_extra_chip_unlocked",
    @"achievement_type_goals_completed",
    @"achievement_type_played_every_day",
    @"achievement_type_half_screen_play",
    @"achievement_type_lose_at_666",
    @"achievement_type_sos",
    @"achievement_type_all_chips_touching_cushions",
    @"achievement_type_smiley",
    @"achievement_type_all_skill_shots_achieved",
    @"achievement_type_hearts_earned",
    @"achievement_type_hearts_used",
    @"achievement_type_supercharged",
    @"achievement_type_chip_type_fast_upgraded",
    @"achievement_type_chip_type_extra_points_upgraded",
    @"achievement_type_chip_type_swap_upgraded",
    @"achievement_type_chip_type_extra_chip_upgraded",
    @"achievement_type_score_points"
};

- (instancetype)initWithDelegate:(id<GameCenterDelegate>)delegate {
    if (self = [super init]) {
        _delegate = delegate;
        _score = 0;
        __weak GameCenter *weakSelf = self;

        GKLocalPlayer.localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError *error) {
            __strong GameCenter *strongSelf = weakSelf;
            if (error) {
                [strongSelf->_delegate gameCenterDidFailWithError:error.localizedDescription];
            }
            if (viewController != nil) {
                strongSelf->_authenticateViewController = viewController;
            } else if (GKLocalPlayer.localPlayer.isAuthenticated) {
                if (strongSelf->_score > 0) {
                    [strongSelf reportScore:strongSelf->_score];
                }
                [strongSelf->_delegate gameCenterDidAuthenticate:YES];
            } else {
                [strongSelf->_delegate gameCenterDidAuthenticate:NO];
            }
        };
    }
    return self;
}

- (void)showAchievements {
    if (_authenticateViewController && !GKLocalPlayer.localPlayer.authenticated) {
        [UIApplication.sharedApplication.keyWindow.rootViewController presentViewController:_authenticateViewController animated:YES completion:nil];
    } else {
        GKGameCenterViewController *gameCenterController = [GKGameCenterViewController new];
        gameCenterController.gameCenterDelegate = self;
        gameCenterController.viewState = GKGameCenterViewControllerStateAchievements;
        [UIApplication.sharedApplication.keyWindow.rootViewController presentViewController:gameCenterController animated:YES completion:nil];
    }
}

- (void)showLeaderboard {
    if (_authenticateViewController && !GKLocalPlayer.localPlayer.authenticated) {
        [UIApplication.sharedApplication.keyWindow.rootViewController presentViewController:_authenticateViewController animated:YES completion:nil];
    } else {
        GKGameCenterViewController *gameCenterController = [GKGameCenterViewController new];
        gameCenterController.gameCenterDelegate = self;
        gameCenterController.viewState = GKGameCenterViewControllerStateLeaderboards;
        gameCenterController.leaderboardIdentifier = kLeaderboardId;
        [UIApplication.sharedApplication.keyWindow.rootViewController presentViewController:gameCenterController animated:YES completion:nil];
    }
}

- (void)loadLeaderboardData {
    [self loadLeaderboardDataForTimeScope:GKLeaderboardTimeScopeToday];
    [self loadLeaderboardDataForTimeScope:GKLeaderboardTimeScopeWeek];
    [self loadLeaderboardDataForTimeScope:GKLeaderboardTimeScopeAllTime];
}

- (void)setAchievementsProgress:(const float *)progress {
    NSMutableArray<GKAchievement *> *achievements = [NSMutableArray new];

    for (unsigned short i = 0; i < achievement_type_range; ++i) {
        if (progress[i] > 1e-5f) {
            GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:kAchievementIds[i]];
            if (achievement) {
                achievement.showsCompletionBanner = YES;
                achievement.percentComplete = progress[i] * 100.f;
                [achievements addObject:achievement];
            }
        }
    }
    if (achievements.count) {
        __weak GameCenter *weakSelf = self;
        [GKAchievement reportAchievements:achievements withCompletionHandler:^(NSError *error) {
             if (error) {
                 __strong GameCenter *strongSelf = weakSelf;
                 [strongSelf->_delegate gameCenterDidFailWithError:error.localizedDescription];
             }
         }];
    }
}

- (void)reportScore:(int64_t)score {
    if (GKLocalPlayer.localPlayer.authenticated) {
        _score = 0;
        GKScore *s = [[GKScore alloc] initWithLeaderboardIdentifier:kLeaderboardId];
        s.value = score;
        s.shouldSetDefaultLeaderboard = YES;

        __weak GameCenter *weakSelf = self;
        [GKScore reportScores:@[s] withCompletionHandler:^(NSError *error) {
            if (error) {
                __strong GameCenter *strongSelf = weakSelf;
                [strongSelf->_delegate gameCenterDidFailWithError:error.localizedDescription];
            }
        }];
    } else {
        _score = score;
    }
}

- (BOOL)authenticated {
    return GKLocalPlayer.localPlayer.authenticated;
}

// MARK: - GKGameCenterControllerDelegate

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [gameCenterViewController dismissViewControllerAnimated:YES completion:nil];
}

// MARK: -

- (void)loadLeaderboardDataForTimeScope:(GKLeaderboardTimeScope)timeScope {
    GKLeaderboard *leaderboard = [GKLeaderboard new];
    leaderboard.identifier = kLeaderboardId;
    leaderboard.timeScope = timeScope;

    __weak GKLeaderboard *weakLeaderboard = leaderboard;
    __weak GameCenter *weakSelf = self;

    [leaderboard loadScoresWithCompletionHandler:^(NSArray<GKScore *> *scores, NSError * error) {
        __strong GameCenter *strongSelf = weakSelf;
        if (error) {
            [strongSelf->_delegate gameCenterDidFailWithError:error.localizedDescription];
        } else {
            __strong GKLeaderboard *strongLeaderboard = weakLeaderboard;
            [strongSelf->_delegate gameCenterDidRetrieveRank:strongLeaderboard.localPlayerScore.rank
                                                forTimeScope:timeScope];
        }
    }];
}

// Note: Uncomment to reset progress during testing
//- (void)resetAchievements {
//    __weak GameCenter *weakSelf = self;
//    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError * _Nullable error) {
//        if (error) {
//            __strong GameCenter *strongSelf = weakSelf;
//            [strongSelf->_delegate gameCenterDidFailWithError:error.localizedDescription];
//        }
//    }];
//}

@end
