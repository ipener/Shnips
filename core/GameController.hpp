// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include "app_state.hpp"
#include "assets.hpp"
#include "IAchievementsHandlerDelegate.hpp"
#include "in_app_purchase_type.h"
#include "ISingleplayerHandlerDelegate.hpp"
#include "IStoreHandlerDelegate.hpp"
#include "ITutorialHandlerDelegate.hpp"
#include "IView.hpp"

template<typename ... T>
struct ViewController : IViewDelegate<T>... {
    ViewController() : IViewDelegate<T>()... {}
};

class GameController : public ISingleplayerHandlerDelegate, public IStoreHandlerDelegate, public ITutorialHandlerDelegate, public IAchievementsHandlerDelegate,
    ViewController<
        class HomeView,
        class HighscoreView,
        class GoalsView,
        class GoalCompletedView,
        class UnlockedView,
        class TutorialView,
        class AlertView,
        class MenuView,
        class GameOverView,
        class HeartsView,
        class TwitterView,
        class StoreView,
        class VideoView
    > {
public:
    GameController(class IAppDelegate *app_delegate, float scale_factor);

    ~GameController();

    void init();

    void appDidBecomeActive();

    void appWillResignActive();

    void update(float t);

    void touchesBegan(double x, double y, long long t_ms);

    void touchesMoved(double x, double y, long long t_ms);

    void touchesEnded(double x, double y, long long t_ms);

    void didRetrieveRank(unsigned long long rank, unsigned short index);

    void didRetrievePurchase(const char *price, in_app_purchase_type type);

    void didBuyInAppPurchase(in_app_purchase_type type);

    void didCancelInAppPurchase();

    void setAuthenticated(bool authenticated);

    // iOS only: used to suspend and restore audio context during audio interrupts (e.g. calls)
    void didReceiveAudioInterruptNotification(bool start);

    // Android only: utilizing the native back button
    void didTapBackButton();

private:
    // MARK: - ISingleplayerHandlerDelegate
    void setHighscore(unsigned long long score, bool post_to_leaderboard) override;

    void setScore(unsigned long long score) override;

    void setShotScore(unsigned long long score) override;

    void setNotification(skill_shot_type type) override;

    void setNotificationAlpha(float alpha) override;

    void setUpgradeNotificationAlpha(float alpha) override;

    void setBarInfoAlpha(float alpha) override;

    void setButtonInfoAlpha(float alpha) override;

    void setSuperchargerAlpha(float alpha) override;

    void addChipProgress(unsigned long long progress) override;

    void setProgress(unsigned long long progress, unsigned long long count) override;

    void setGoal(unsigned short goal) override;

    void setVideoType(video_type type) override;

    void showHomeView() override;

    void showHighscoreView() override;

    void showGoalCompletedView() override;

    void showGameOverView(unsigned long long score, bool new_highscore_reached) override;

    void showIntro(intro_type type) override;

    void updateHeartsAlpha() override;

    // MARK: - IStoreHandlerDelegate
    void setBackgroundData(const float gradient[6], float blur_intensity) override;

    void setChipType(chip_type type, unsigned short level, float progress) override;

    void setGoalsProgress(unsigned short total) override;

    void setIndicatorLevel(unsigned short index, unsigned short level) override;

    void setIndicatorAlpha(unsigned short index, float alpha) override;

    void setViewAlpha(float alpha) override;

    void didChangePage() override;

    // MARK: - ITutorialHandlerDelegate
    void setCancelable(bool cancelable) override;

    void setTutorialScore(unsigned long long score) override;

    void setTutorialShotScore(unsigned long long score) override;

    void setTutorialTitle(tutorial_type type) override;

    void setTutorialTitleAlpha(float alpha) override;

    void exitTutorial() override;

    void showAlert(tutorial_type type) override;

    // MARK: - IAchievementsHandlerDelegate
    void achievementUnlocked(achievement_type type) override;

    void setAchievementsProgress(const float progress[achievement_type_range]) override;

    // MARK: - IViewDelegate
    void tapped(class HomeView *view, const vec2<float> &p) override;

    void tapped(class HighscoreView *view, const vec2<float> &p) override;

    void tapped(class GoalsView *view, const vec2<float> &p) override;

    void tapped(class GoalCompletedView *view, const vec2<float> &p) override;

    void tapped(class UnlockedView *view, const vec2<float> &p) override;

    void tapped(class TutorialView *view, const vec2<float> &p) override;

    void tapped(class AlertView *view, const vec2<float> &p) override;

    void tapped(class MenuView *view, const vec2<float> &p) override;

    void tapped(class GameOverView *view, const vec2<float> &p) override;

    void tapped(class HeartsView *view, const vec2<float> &p) override;

    void tapped(class TwitterView *view, const vec2<float> &p) override;

    void tapped(class StoreView *view, const vec2<float> &p) override;

    void tapped(class VideoView *view, const vec2<float> &p) override;

    // MARK: -
    void setPaused(bool paused);

    void save();

    void startTransition(class IView *next_view);

    void setTransitionProgress(class IView *view, const vec2<float> &transition_data);

    void setMaxContinuedGameCount();

    void setHeartsAlpha(float alpha);

    void setHeartButtonEnabled(bool enabled);

    void showMenuView(bool in_game);

    void showStoreView();

    void share();

    void tapped(const vec2<float> &p);

    void startPurchase(in_app_purchase_type type);

    bool shouldAskToRate();

    bool inGameView(class IView *view) const;

    bool hasHearts() const;

    bool shouldPropagateTouches() const;

    bool inInteractiveView() const;

private:
    const unsigned short       _max_consecutive_rate_prompts = 5;
    const unsigned short       _user_rated_already = 0xFFFF;

    class Scene               *_scene = nullptr;
    class ALRenderer          *_al_renderer = nullptr;
    class GLRenderer          *_gl_renderer = nullptr;
    class AchievementsHandler *_achievements_handler = nullptr;
    class SingleplayerHandler *_singleplayer_handler = nullptr;
    class StoreHandler        *_store_handler = nullptr;
    class TutorialHandler     *_tutorial_handler = nullptr;
    class VideoHandler        *_video_handler = nullptr;
    class IAppDelegate        *_app_delegate = nullptr;
    class IView               *_background_view = nullptr;
    class IView               *_current_view = nullptr;
    class IView               *_next_view = nullptr;

    app_state                  _state;
    vec2<double>               _touches_begin_point;
    // f(t) = _transition_data.x * t + _transition_data.y
    vec2<float>                _background_view_transition_data;
    vec2<float>                _current_view_transition_data;
    vec2<float>                _store_transition_data;
    vec2<float>                _video_transition_data;

    long long                  _begin_touch_time;
    float                      _height;
    float                      _transition_coeff = 1.f;
    float                      _ui_transition_progress = 0.f;
    unsigned short             _twitter_prompt_count = 10;
    bool                       _ui_transition_running = false;
    bool                       _invalid_touch = false;
    bool                       _is_tap = false;
    bool                       _first_time_tutorial = false;
    bool                       _in_purchase = false;
    bool                       _active = false;
    bool                       _back_button_tapped = false;
    bool                       _scheduled_notifications = false;
    bool                       _prompted_for_notifications = false;
    bool                       _hearts_info_shown = false;
};

#endif
