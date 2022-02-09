// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IAPPDELEGATE_HPP
#define IAPPDELEGATE_HPP

#include "achievement_type.h"
#include "assets.hpp"
#include "in_app_purchase_type.h"
#include "IResourceLoader.hpp"
#include "IStateHandler.hpp"
#include "notification_type.h"

class IAppDelegate : public IStateHandler, public IResourceLoader {
public:
    IAppDelegate(float width, float height) {
        assets::screen_size = {width, height};
        assets::loader = this;
    }

    virtual ~IAppDelegate() {}

    virtual void setPaused(bool paused) = 0;

    virtual void setAchievementsProgress(const float progress[achievement_type_range]) = 0;

    virtual void setLeaderboardData(unsigned long long score) = 0;

    virtual void getLeaderboardData() = 0;

    virtual void showAppRatingView() = 0;

    virtual void showTwitterWebsite() = 0;
    
    virtual void showAchievements() = 0;

    virtual void showLeaderboard() = 0;

    virtual bool authenticatedForLeaderboard() = 0;

    virtual void startPurchase(in_app_purchase_type type) = 0;

    virtual void share(const char *str) = 0;

    virtual void scheduleNotifications(const char *titles[notification_type_range]) = 0;

    virtual void cancelNotifications() = 0;

    // iOS only: must be implemented to comply with Apple's policies
    virtual void restorePurchases() {};

    // iOS only: used to ask for permission to send notifications
    virtual void promptForNotificationPermission() {}

    // Android only: used for the Android back button.
    virtual void showQuitAppAlert() {}

    // Android only: ensuring that Google Play store is initialized after GameController.
    virtual void setUpInAppBilling() {}
};

#endif
