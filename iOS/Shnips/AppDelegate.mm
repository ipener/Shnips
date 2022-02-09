// Copyright © 2018 Igor Pener. All rights reserved.

#import "AppDelegate.h"

#import <AVFoundation/AVFoundation.h>
#import <StoreKit/StoreKit.h>
#import <UserNotifications/UserNotifications.h>

#import "GameCenter.h"
#import "AppDelegateBridge.h"
#import "AppStore.h"

#include <OpenGLES/ES3/glext.h>
#include "GameController.hpp"

@interface FullScreenViewController : UIViewController

@end

@implementation FullScreenViewController

- (BOOL)prefersStatusBarHidden {
    return YES;
}

@end

@interface AppDelegate () <UNUserNotificationCenterDelegate, AppStoreDelegate, GameCenterDelegate> {
    GameCenter     *_gameCenter;
    AppStore       *_appStore;
    UITouch        *_touch;
    NSDate         *_launchTime;
    CADisplayLink  *_displayLink;
    EAGLContext    *_context;
    
    GameController *_gameController;
    GLuint         _framebuffer;
    GLuint         _renderbuffer;
}

@end

@implementation AppDelegate

- (void)dealloc {
    if (_displayLink) {
        [_displayLink invalidate];
    }
}

// MARK: - UIApplicationDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self setUp];
    UNUserNotificationCenter.currentNotificationCenter.delegate = self;
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    if (_gameController) {
        _gameController->appWillResignActive();
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    if (_gameController) {
        _gameController->appWillResignActive();
    }
    // Note: Uncomment to simulate a memory warning
    //[UIApplication.sharedApplication performSelector:@selector(_performMemoryWarning)];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [self setUp];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (_gameController) {
        _gameController->appDidBecomeActive();
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    if (_gameController) {
        _gameController->appWillResignActive();
    }
    [self cleanUp];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    [self cleanUp];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    if (_touch)
        return;

    _touch = touches.anyObject;
    if (_gameController) {
        const CGPoint location = [_touch locationInView:self.window];
        const long long t_ms = static_cast<long long>(event.timestamp * assets::ms_for_1s);
        _gameController->touchesBegan(location.x, location.y, t_ms);
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    if (_touch && [touches containsObject:_touch]) {
        if (_gameController) {
            const CGPoint location = [_touch locationInView:self.window];
            const long long t_ms = static_cast<long long>(event.timestamp * assets::ms_for_1s);
            _gameController->touchesMoved(location.x, location.y, t_ms);
        }
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    if (_touch) {
        if (_gameController) {
            const CGPoint location = [_touch locationInView:self.window];
            const long long t_ms = static_cast<long long>(event.timestamp * assets::ms_for_1s);
            _gameController->touchesEnded(location.x, location.y, t_ms);
        }
        _touch = nil;
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

// MARK: - UNUserNotificationCenterDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
didReceiveNotificationResponse:(UNNotificationResponse *)response
         withCompletionHandler:(void (^)())completionHandler {
    completionHandler();
}

// MARK: - StoreDelegate

- (void)appStoreDidRetrieveInAppPurchase:(in_app_purchase_type)type withPrice:(NSString *)price {
    if (_gameController) {
        _gameController->didRetrievePurchase(price.UTF8String, type);
    }
}

- (void)appStoreDidFailWithError:(NSString *)log {
    if (_gameController) {
        _gameController->didCancelInAppPurchase();
    }
    logi << log.UTF8String << std::endl;
}

- (void)appStoreDidBuyInAppPurchase:(in_app_purchase_type)type {
    if (_gameController) {
        _gameController->didBuyInAppPurchase(type);
    }
}

- (void)appStoreDidCancelInAppPurchase {
    if (_gameController) {
        _gameController->didCancelInAppPurchase();
    }
}

// MARK: - GameCenterDelegate

- (void)gameCenterDidFailWithError:(NSString *)log {
    logi << log.UTF8String << std::endl;
}

- (void)gameCenterDidRetrieveRank:(NSInteger)rank forTimeScope:(GKLeaderboardTimeScope)timeScope {
    if (_gameController) {
        _gameController->didRetrieveRank(rank, timeScope);
    }
}

- (void)gameCenterDidAuthenticate:(BOOL)authenticated; {
    if (_gameController) {
        _gameController->setAuthenticated(authenticated);
    }
}

// MARK: -

- (void)update:(CADisplayLink *)displayLink {
    if (_gameController) {
        _gameController->update(_displayLink.duration);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)setPaused:(BOOL)paused {
    _displayLink.paused = paused;
}

- (void)setAchievementsProgress:(const float *)progress {
    if (_gameCenter) {
        [_gameCenter setAchievementsProgress:progress];
    }
}

- (void)setLeaderboardData:(unsigned long long)score {
    if (_gameCenter) {
        [_gameCenter reportScore:score];
    }
}

- (void)getLeaderboardData {
    if (_gameCenter) {
        [_gameCenter loadLeaderboardData];
    }
}

- (void)showAppRatingView {
    NSURL *url = [NSURL URLWithString:@"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?id=1154187411&onlyLatestVersion=true&pageNumber=0&sortOrdering=1&type=Purple+Software"];
    [UIApplication.sharedApplication openURL:url options:@{} completionHandler:nil];
}

- (void)showTwitterWebsite {
    NSURL *url = [NSURL URLWithString:@"https://twitter.com/shnipsgame"];
    [UIApplication.sharedApplication openURL:url options:@{} completionHandler:nil];
}

- (void)showAchievements {
    if (_gameCenter) {
        [_gameCenter showAchievements];
    }
}

- (void)showLeaderboard {
    if (_gameCenter) {
        [_gameCenter showLeaderboard];
    }
}

- (BOOL)authenticatedForLeaderboard {
    return _gameCenter && _gameCenter.authenticated;
}

- (void)startPurchase:(in_app_purchase_type)type {
    [_appStore startPurchase:type];
}

- (void)share:(const char *)str {
    if ([UIScreen.mainScreen respondsToSelector:@selector(scale)]) {
        UIGraphicsBeginImageContextWithOptions(self.window.bounds.size, NO, UIScreen.mainScreen.scale);
    } else {
        UIGraphicsBeginImageContext(self.window.bounds.size);
    }
    [self.window.rootViewController.view drawViewHierarchyInRect:self.window.bounds
                                              afterScreenUpdates:YES];

    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();

    UIActivityViewController *vc = [[UIActivityViewController alloc] initWithActivityItems:@[image, [NSString stringWithUTF8String:str]]
                                                                           applicationActivities:nil];
    vc.excludedActivityTypes = @[
        UIActivityTypePrint,
        UIActivityTypeCopyToPasteboard,
        UIActivityTypeAssignToContact,
        UIActivityTypeSaveToCameraRoll,
        UIActivityTypeAddToReadingList,
        UIActivityTypePostToFlickr,
        UIActivityTypePostToVimeo,
        UIActivityTypePostToTencentWeibo,
        UIActivityTypeAirDrop,
        UIActivityTypeOpenInIBooks
    ];
    [self.window.rootViewController presentViewController:vc animated:YES completion:nil];
}

- (void)scheduleNotifications:(const char **)titles {
    const NSTimeInterval timeIntervalSinceNow = _launchTime.timeIntervalSinceNow;
    void(^scheduleNotification)(notification_type) = ^(notification_type type) {
        if (titles[type]) {
            const NSTimeInterval interval = (1 << type) * 24 * 60 * 60 - timeIntervalSinceNow;
            NSString *identifier = [NSString stringWithUTF8String:assets::loader->key(type).c_str()];
            UNMutableNotificationContent *content = [UNMutableNotificationContent new];
            content.body = [NSString stringWithUTF8String:titles[type]];
            content.sound = UNNotificationSound.defaultSound;
            UNNotificationTrigger *trigger = [UNTimeIntervalNotificationTrigger triggerWithTimeInterval:interval repeats:NO];
            UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:identifier content:content trigger:trigger];
            [UNUserNotificationCenter.currentNotificationCenter addNotificationRequest:request withCompletionHandler:^(NSError * _Nullable error) {
                if (error) {
                    logi << error.localizedDescription.UTF8String << std::endl;
                }
            }];
        }
    };

    scheduleNotification(notification_type_goals_left);
    scheduleNotification(notification_type_goal);
    scheduleNotification(notification_type_highscore);
}

- (void)cancelNotifications {
    _launchTime = [NSDate date];
    [UNUserNotificationCenter.currentNotificationCenter removeAllPendingNotificationRequests];
    [UNUserNotificationCenter.currentNotificationCenter removeAllDeliveredNotifications];
}

- (void)restorePurchases {
    [_appStore restorePurchases];
}

- (void)promptForNotificationPermission {
    UNUserNotificationCenter *center = UNUserNotificationCenter.currentNotificationCenter;
    [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert + UNAuthorizationOptionSound)
                          completionHandler:^(BOOL granted, NSError * _Nullable error) {
                          }];
}

// MARK: -

- (void)setUp {
    _touch = nil;
    _launchTime = [NSDate date];
    if (self.window == nil) {
        _framebuffer = _renderbuffer = 0;
        CGRect bounds = UIScreen.mainScreen.bounds;
         // Note: Insetting bounds to avoid notch cutout on iPhone X etc.
        if (fabs(bounds.size.height - 812.f) < 1e-5f || fabs(bounds.size.height - 896.f) < 1e-5f) {
            bounds.size.height -= 2.f * 40.f;
            bounds.origin.y += 40.f;
        }
        self.window = [[UIWindow alloc] initWithFrame:bounds];
        self.window.contentScaleFactor = UIScreen.mainScreen.scale;
        self.window.clipsToBounds = YES;
        [self.window makeKeyAndVisible];
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(update:)];
        [_displayLink addToRunLoop:NSRunLoop.currentRunLoop forMode:NSDefaultRunLoopMode];
    }
    if (!_framebuffer) {
        CAEAGLLayer *layer = [CAEAGLLayer new];
        layer.presentsWithTransaction = YES;
        layer.frame = self.window.bounds;
        layer.contentsScale = self.window.contentScaleFactor;
        layer.drawableProperties = @{
            kEAGLDrawablePropertyRetainedBacking: @(NO),
            kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8
        };

        self.window.rootViewController = [FullScreenViewController new];
        UIView *view = self.window.rootViewController.view;
        view.frame = self.window.bounds;
        view.contentScaleFactor = self.window.contentScaleFactor;
        view.multipleTouchEnabled = NO;
        [view.layer addSublayer:layer];

        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (!_context) {
            loge << "Failed to create ES3 context" << std::endl;
        }

        [EAGLContext setCurrentContext:_context];

        glGenFramebuffers(1, &_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

        glGenRenderbuffers(1, &_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffer);

        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderbuffer);
        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            logi << "glCheckFramebufferStatus returned " << status << std::endl;
        }

        _gameController = new GameController(
            new AppDelegateBridge(self, (float)self.window.bounds.size.width, (float)self.window.bounds.size.height),
            (float)self.window.contentScaleFactor
        );
        _gameController->init();

        _appStore = [[AppStore alloc] initWithDelegate:self];
        _gameCenter = [[GameCenter alloc] initWithDelegate:self];

        NSError *error = nil;
        [AVAudioSession.sharedInstance setCategory:AVAudioSessionCategoryAmbient error:&error];
        if (error) {
            logi << error.localizedDescription.UTF8String << std::endl;
        }
        [NSNotificationCenter.defaultCenter addObserver:self
                                               selector:@selector(didReceiveInterruptionNotification:)
                                                   name:AVAudioSessionInterruptionNotification
                                                 object:AVAudioSession.sharedInstance];
    }
    self.paused = NO;
}

- (void)didReceiveInterruptionNotification:(NSNotification *)notification {
    AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)[[notification.userInfo objectForKey:AVAudioSessionInterruptionTypeKey] unsignedIntegerValue];
    if (type == AVAudioSessionInterruptionTypeBegan) {
        if (_gameController) {
            _gameController->didReceiveAudioInterruptNotification(true);
        }
    } else if (type == AVAudioSessionInterruptionTypeEnded) {
        __weak AppDelegate *weakSelf = self;
        [NSTimer scheduledTimerWithTimeInterval:0.5 repeats:NO block:^(NSTimer * _Nonnull timer) {
            __strong AppDelegate *strongSelf = weakSelf;
            if (strongSelf->_gameController) {
                strongSelf->_gameController->didReceiveAudioInterruptNotification(false);
            }
        }];
    }
}

- (void)cleanUp {
    _displayLink.paused = YES;

    if (_framebuffer) {
        [EAGLContext setCurrentContext:_context];

        glDeleteFramebuffers(1, &_framebuffer);
        glDeleteRenderbuffers(1, &_renderbuffer);
        _framebuffer = _renderbuffer = 0;

        [self.window.rootViewController.view removeFromSuperview];
        self.window.rootViewController.view = nil;
        [self.window.rootViewController removeFromParentViewController];

        if (EAGLContext.currentContext == _context) {
            [EAGLContext setCurrentContext:nil];
        }
        [NSNotificationCenter.defaultCenter removeObserver:self
                                                      name:AVAudioSessionInterruptionNotification
                                                    object:AVAudioSession.sharedInstance];
    }
    self.window.rootViewController = nil;

    if (_gameController) {
        delete _gameController;
        _gameController = NULL;
    }
}

@end
