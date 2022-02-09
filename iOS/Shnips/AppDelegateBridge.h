// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef APPDELEGATEBRIDGE_HPP
#define APPDELEGATEBRIDGE_HPP

#import "AppDelegate.h"

#include "ALModel.hpp"
#include "IAppDelegate.hpp"
#include "Logger.hpp"

class AppDelegateBridge : public IAppDelegate {
public:
    AppDelegateBridge(AppDelegate *delegate, float width, float height) : IAppDelegate(width, height), _delegate(delegate) {
    }

    ~AppDelegateBridge() override {
        _delegate = nil;
    }

    // MARK: - IStateHandler
    void reset() override {
        NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
        NSDictionary *dict = defaults.dictionaryRepresentation;
        for (id key in dict) {
            [defaults removeObjectForKey:key];
        }
        [defaults synchronize];
    }

    void save(const std::string &state, const std::string &key) override {
        NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
        [defaults setObject:[NSString stringWithUTF8String:state.c_str()] forKey:[NSString stringWithUTF8String:key.c_str()]];
    }

    bool load(std::string &state, const std::string &key) override {
        NSUserDefaults *defaults = NSUserDefaults.standardUserDefaults;
        NSString *data = [defaults objectForKey:[NSString stringWithUTF8String:key.c_str()]];

        if (!data)
            return false;

        state = std::string(data.UTF8String);
        return true;
    }

    // MARK: - IResourceLoader
    std::wstring localizedString(const char *key) override {
        NSString *str = [NSBundle.mainBundle localizedStringForKey:[NSString stringWithUTF8String:key] value:nil table:nil];
        return converter.from_bytes(str.UTF8String);
    }

    void loadImage(void **data, int *width, int *height, const char *filename) override {
        UIImage *image = [UIImage imageNamed:[NSString stringWithUTF8String:filename]];
        CGDataProviderRef provider = CGImageGetDataProvider(image.CGImage);
        NSData *nsdata = (id)CFBridgingRelease(CGDataProviderCopyData(provider));

        if (nsdata) {
            const auto bytes = static_cast<GLsizei>(nsdata.length);
            *width = static_cast<GLsizei>(image.size.width);
            *height = static_cast<GLsizei>(image.size.height);
            *data = malloc(bytes);
            [nsdata getBytes:*data length:bytes];
        } else {
            loge << "loadImage() failed to load " << filename << std::endl;
        }
    }

    void loadAudio(void **data, int *size, const char *filename) override {
        NSString *path = [NSBundle.mainBundle pathForResource:[NSString stringWithUTF8String:filename] ofType:@"sound"];
        NSData *contents = [NSData dataWithContentsOfFile:path];

        if (contents) {
            *size = static_cast<ALsizei>(contents.length);
            *data = malloc(*size);
            const void *bytes = contents.bytes;
            memcpy(*data, bytes, *size);
        } else {
            loge << "loadAudio() failed to load " << filename << std::endl;
        }
    }

    // MARK: - IAppDelegate
    void setPaused(bool paused) override {
        _delegate.paused = paused;
    }

    void setAchievementsProgress(const float progress[achievement_type_range]) override {
        [_delegate setAchievementsProgress:progress];
    }

    void setLeaderboardData(unsigned long long score) override {
        [_delegate setLeaderboardData:score];
    }

    void getLeaderboardData() override {
        [_delegate getLeaderboardData];
    }

    void showAppRatingView() override {
        [_delegate showAppRatingView];
    }

    void showTwitterWebsite() override {
        [_delegate showTwitterWebsite];
    }

    void showAchievements() override {
        [_delegate showAchievements];
    }

    void showLeaderboard() override {
        [_delegate showLeaderboard];
    }

    bool authenticatedForLeaderboard() override {
        return _delegate.authenticatedForLeaderboard;
    }

    void startPurchase(in_app_purchase_type type) override {
        [_delegate startPurchase:type];
    }

    void share(const char *str) override {
        [_delegate share:str];
    }

    void scheduleNotifications(const char *titles[notification_type_range]) override {
        [_delegate scheduleNotifications:titles];
    }

    void cancelNotifications() override {
        [_delegate cancelNotifications];
    }

    void restorePurchases() override {
        [_delegate restorePurchases];
    }

    void promptForNotificationPermission() override {
        [_delegate promptForNotificationPermission];
    }

private:
    AppDelegate *_delegate;
};

#endif
