// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef APPDELEGATEBRIDGE_HPP
#define APPDELEGATEBRIDGE_HPP

#include <jni.h>
#include "IAppDelegate.hpp"

class GameController;

class AppDelegateBridge : public IAppDelegate {
public:
    struct JNI {
        ~JNI();

        // This method should be called by JNI_OnLoad
        jint init(JavaVM *vm);

        JNIEnv *getEnv();

        JavaVM   *vm = nullptr;
        jclass    obj = nullptr;
        jmethodID reset = nullptr;
        jmethodID save = nullptr;
        jmethodID load = nullptr;
        jmethodID localizedString = nullptr;
        jmethodID loadImage = nullptr;
        jmethodID loadAudio = nullptr;
        jmethodID setPaused = nullptr;
        jmethodID setAchievementsProgress = nullptr;
        jmethodID setLeaderboardData = nullptr;
        jmethodID getLeaderboardData = nullptr;
        jmethodID showAppRatingView = nullptr;
        jmethodID showTwitterWebsite = nullptr;
        jmethodID showAchievements = nullptr;
        jmethodID showLeaderboard = nullptr;
        jmethodID authenticatedForLeaderboard = nullptr;
        jmethodID startPurchase = nullptr;
        jmethodID share = nullptr;
        jmethodID scheduleNotifications = nullptr;
        jmethodID cancelNotifications = nullptr;
        jmethodID showQuitAppAlert = nullptr;
        jmethodID setUpInAppBilling = nullptr;
    };

    AppDelegateBridge(float width, float height);

    // MARK: - IStateHandler
    void reset() override;

    void save(const std::string &data, const std::string &key) override;

    bool load(std::string &data, const std::string &key) override;

    // MARK: - IResourceLoader
    std::wstring localizedString(const char *key) override;

    void loadImage(void **data, int *width, int *height, const char *filename) override;

    void loadAudio(void **data, int *bytes, const char *filename) override;

    void setPaused(bool paused) override;

    void setAchievementsProgress(const float progress[achievement_type_range]) override;

    void setLeaderboardData(unsigned long long score) override;

    void getLeaderboardData() override;

    void showAppRatingView() override;

    void showTwitterWebsite() override;

    void showAchievements() override;

    void showLeaderboard() override;

    bool authenticatedForLeaderboard() override;

    void startPurchase(in_app_purchase_type type) override;

    void share(const char *str) override;

    void scheduleNotifications(const char *titles[notification_type_range]) override;

    void cancelNotifications() override;

    void showQuitAppAlert() override;

    void setUpInAppBilling() override;

    static JNI             jni;
    static GameController *game_controller;
};

#endif
