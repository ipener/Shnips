// Copyright © 2018 Igor Pener. All rights reserved.

#include "GameController.hpp"

#include <exception>
#include "AchievementsHandler.hpp"
#include "ALRenderer.hpp"
#include "GameOverView.hpp"
#include "GLRenderer.hpp"
#include "GoalCompletedView.hpp"
#include "GoalsView.hpp"
#include "HeartsView.hpp"
#include "HighscoreView.hpp"
#include "HomeView.hpp"
#include "IAppDelegate.hpp"
#include "IResourceLoader.hpp"
#include "IStateHandler.hpp"
#include "Logger.hpp"
#include "MenuView.hpp"
#include "Scene.hpp"
#include "SingleplayerHandler.hpp"
#include "StoreHandler.hpp"
#include "StoreView.hpp"
#include "TutorialHandler.hpp"
#include "AlertView.hpp"
#include "TutorialView.hpp"
#include "TwitterView.hpp"
#include "UnlockedView.hpp"
#include "utils.hpp"
#include "VideoHandler.hpp"
#include "VideoView.hpp"

using namespace std;
using namespace assets;

GameController::GameController(IAppDelegate *app_delegate, float scale_factor) : _app_delegate(app_delegate) {
    srand(static_cast<unsigned int>(time(NULL)));

    _al_renderer = new ALRenderer(_app_delegate);
    _gl_renderer = new GLRenderer(_app_delegate, scale_factor);
    _scene = new Scene(_gl_renderer, _al_renderer, &_state.base_state);
    _tutorial_handler = new TutorialHandler(&_state.base_state, _app_delegate);
    _store_handler = new StoreHandler(&_state.base_state, _app_delegate);
    _singleplayer_handler = new SingleplayerHandler(&_state.base_state, _app_delegate);
    _video_handler = new VideoHandler(&_state.base_state);

    _achievements_handler = new AchievementsHandler(&_state.base_state, &_singleplayer_handler->state(), _app_delegate);
    achievements_handler = _achievements_handler;

    _height = screen_size.y;
}

GameController::~GameController() {
    try {
        delete _scene;
        delete _al_renderer;
        delete _gl_renderer;
        delete _singleplayer_handler;
        delete _store_handler;
        delete _tutorial_handler;
        delete _video_handler;
        delete _achievements_handler;
        delete _app_delegate;
    } catch (const exception &e) {
        cerr << "Exception occured in ~GameController: " << e.what() << endl;
    }
}

void GameController::init() {
    if (!_app_delegate->load(&_state) && _state.base_state.chips_data.empty()) {
        logi << "GameController failed reading state" << endl;
        _app_delegate->reset();
    }
    if (_state.discard_base_state) {
        _state.base_state.chips_data.clear();
    }
    if (_state.base_state.chips_data.empty()) {
        const rect<double> bounds({0.0, 0.0}, {screen_size.x, screen_size.y});
        Chip::defaultChipPositions(default_chips_count, bounds,
            [this](const vec2<double> &p) {
                _state.base_state.chips_data.push_back({p, {}});
            }
        );
    }

    _achievements_handler->init(this);
    _tutorial_handler->init(this, _scene);
    _singleplayer_handler->init(this, _scene);
    _store_handler->init(this, _singleplayer_handler->state().goal);
    _video_handler->init(_scene);
    _scene->init();
    _gl_renderer->loadGLResources();
    _gl_renderer->setBackgroundGradient(chip_themes[_state.base_state.chip_type].gradient);

    if (_tutorial_handler->state().completed) {
        _scene->setGameHandler(_singleplayer_handler);
    } else {
        _scene->setGameHandler(_tutorial_handler);
    }
    setMaxContinuedGameCount();

    if (hasHearts()) {
        setHeartButtonEnabled(false);
    } else if (_singleplayer_handler->state().goal >= goals_for_hearts_purchase) {
        _hearts_info_shown = true;
        setHeartButtonEnabled(true);
        setButtonInfoAlpha(1.f);
        _singleplayer_handler->setHeartTargetAlpha(1.f);
    }

    IViewDelegate<HomeView>::view->setGoals(_singleplayer_handler->state().goal, _store_handler->goalsToUnlockNextChip());
    if (_store_handler->canShowGoals()) {
        IViewDelegate<HomeView>::view->setGoalsAlpha(_singleplayer_handler->state().goal < goals_count ? 1.f : ui_disabled_alpha);
    }
    _achievements_handler->beginShot();

    if (_state.muted) {
        _al_renderer->mute();
    }
    IViewDelegate<MenuView>::view->setAchievements(_achievements_handler->achieved(), achievement_type_range);
    IViewDelegate<MenuView>::view->setMuted(_state.muted);

    if (_singleplayer_handler->gameStarted()) {
        if (_singleplayer_handler->state().game_type == game_type::highscore) {
            _current_view = IViewDelegate<HighscoreView>::view;
        } else {
            _current_view = IViewDelegate<GoalsView>::view;
        }
        if (_state.base_state.chip_type != chip_type_default) {
            _scene->setChargeAlpha(1.f);
        }
        setHeartsAlpha(1.f);
    } else if (!_tutorial_handler->state().completed) {
        _current_view = IViewDelegate<TutorialView>::view;
        _tutorial_handler->showNextTutorial();
    } else {
        _current_view = IViewDelegate<HomeView>::view;
        _singleplayer_handler->showCue();
    }
    _current_view->setTransitionProgress(0.f);
    _scene->setFullScreenView(_current_view);
    _app_delegate->setUpInAppBilling();
}

void GameController::appDidBecomeActive() {
    if (!_active) {
        _app_delegate->cancelNotifications();
        _scheduled_notifications = false;
        _active = true;
        if (_al_renderer) {
            setPaused(false);
        }
    }
}

void GameController::appWillResignActive() {
    if (_active) {
        if (_current_view == IViewDelegate<VideoView>::view) {
            _ui_transition_progress = 1.f;
            _ui_transition_running = false;

            _next_view = nullptr;
            _current_view->setTransitionProgress(1.f);
            _current_view = IViewDelegate<GoalsView>::view;
            _current_view->setTransitionProgress(0.f);
            _scene->setFullScreenView(_current_view);
            if (_state.base_state.chip_type != chip_type_default) {
                _scene->setChargeAlpha(1.f);
            }
            setHeartsAlpha(1.f);
            _scene->setBackgroundBlending(true);
            _scene->setGameHandler(_singleplayer_handler);
            _video_transition_data = {0.f, 0.f};
            _state.discard_base_state = false;
        }
        if (!_scheduled_notifications) {
            _scheduled_notifications = true;

            std::string goals_left_str, goal_str;
            const auto type = _store_handler->nextUnlockableChip();
            const auto goal = _singleplayer_handler->state().goal;
            if (type != chip_type_range) {
                const auto n = _store_handler->goalsToUnlockNextChip() - _singleplayer_handler->state().goal;
                const auto chip = loader->localizedString(loader->key(type).c_str());
                if (n == 1) {
                    goals_left_str = loader->str(loader->localizedString(loader->key(notification_type_goal).c_str(), chip));
                } else if (n < 4) {
                    goals_left_str = loader->str(loader->localizedString(loader->key(notification_type_goals_left).c_str(), goal, chip));
                }
            }
            if (goal < goals_count) {
                const auto key = "goal_" + std::to_string(goal);
                goal_str = loader->clean_str(loader->localizedString(key.c_str()));
            }
            const auto highscore_str = loader->str(loader->localizedString(loader->key(notification_type_highscore).c_str(), _singleplayer_handler->state().highscore));

            const char *titles[notification_type_range] {
                goals_left_str.empty() ? nullptr : goals_left_str.c_str(),
                goal_str.empty() ? nullptr : goal_str.c_str(),
                highscore_str.c_str()
            };
            _app_delegate->scheduleNotifications(titles);
        }
        _active = false;
        save();
        if (_al_renderer) {
            setPaused(true);
            _al_renderer->freeContext();
        }
    }
}

void GameController::update(float t) {
    if (_in_purchase)
        return;

    if (_ui_transition_running) {
        _ui_transition_progress += t * ui_transition_step * _transition_coeff;

        if (_ui_transition_progress > 0.99999f) {
            _ui_transition_progress = 1.f;
            _ui_transition_running = false;
        } else if (_ui_transition_progress > 0.5f && _next_view) {
            if (_background_view) {
                if (_next_view->type() != view_type::full_screen) {
                    _current_view->setTransitionProgress(1.f);
                    _current_view = _next_view;
                    _scene->setAlertView(_current_view);
                } else {
                    _background_view = nullptr;
                    _current_view->setTransitionProgress(1.f);
                    _current_view = _next_view;
                    _scene->setAlertView(nullptr);
                    _scene->setFullScreenView(_current_view);
                    _scene->setChargeAlpha(0.f);
                    setHeartsAlpha(0.f);
                    _background_view_transition_data = {0.f, 0.f};
                }
            } else {
                if (_next_view->type() == view_type::full_screen) {
                    if (_current_view == IViewDelegate<HomeView>::view && _next_view == IViewDelegate<GoalsView>::view) {
                        _singleplayer_handler->setGameType(game_type::goals);
                        _singleplayer_handler->startGame();
                    } else if (_current_view == IViewDelegate<GoalsView>::view && _next_view == IViewDelegate<HomeView>::view) {
                        _singleplayer_handler->setGameType(game_type::highscore);
                        _singleplayer_handler->resetGame();
                    }
                    if (inGameView(_current_view)) {
                        _scene->setChargeAlpha(0.f);
                        setHeartsAlpha(0.f);
                    }
                    _current_view->setTransitionProgress(1.f);
                    _current_view = _next_view;
                    _scene->setFullScreenView(_current_view);
                }
                _background_view_transition_data = {0.f, 0.f};
            }
            _current_view_transition_data = {2.f, -2.f};
            _next_view = nullptr;
        }
        setTransitionProgress(_background_view, _background_view_transition_data);
        setTransitionProgress(_current_view, _current_view_transition_data);

        if (fabs(_store_transition_data.x) > 1e-5f) {
            const auto progress = _store_transition_data.x * _ui_transition_progress + _store_transition_data.y;
            if (progress > 0.f) {
                _store_handler->setTransitionProgress(progress);
            } else if (_store_transition_data.x < -1e-5f) {
                _singleplayer_handler->showCue();
                _scene->setGameHandler(_singleplayer_handler);
                _store_transition_data = {0.f, 0.f};
                _state.discard_base_state = false;
            }
        }
        if (fabs(_video_transition_data.x) > 1e-5f) {
            const auto progress = _video_transition_data.x * _ui_transition_progress + _video_transition_data.y;
            if (progress > 0.f) {
                _video_handler->setTransitionProgress(progress);
            } else if (_video_transition_data.x < -1e-5f) {
                _scene->setBackgroundBlending(true);
                _scene->setGameHandler(_singleplayer_handler);
                _video_transition_data = {0.f, 0.f};
                _state.discard_base_state = false;
            }
        }
        if (!_ui_transition_running) {
            if (_background_view && _current_view->type() == view_type::full_screen) {
                _background_view = nullptr;
                _scene->setAlertView(nullptr);
            }
            _store_transition_data = {0.f, 0.f};
            _video_transition_data = {0.f, 0.f};
            if (_current_view == IViewDelegate<GameOverView>::view) {
                _singleplayer_handler->startScoreAnimation();
            } else if (_current_view == IViewDelegate<HomeView>::view) {
                if (_singleplayer_handler->state().highscore > 50 && !_prompted_for_notifications) {
                    _prompted_for_notifications = true;
                    _app_delegate->promptForNotificationPermission();
                } else if (_singleplayer_handler->state().goal > 20 && !_state.followed_on_twitter &&
                           rand() % _twitter_prompt_count == 0) {
                    _twitter_prompt_count++;
                    startTransition(IViewDelegate<TwitterView>::view);
                }
            }
        }
    }

    if (_back_button_tapped) {
        bool back_button_available = false;
        for (const auto &label : _current_view->labels()) {
            if (label.isBackButton()) {
                if (label.alpha > 0.99999f) {
                    tapped(label.p);
                    _back_button_tapped = false;
                }
                back_button_available = true;
                break;
            }
        }
        if (!back_button_available) {
            _app_delegate->showQuitAppAlert();
            _back_button_tapped = false;
        }
    }

    _scene->update(t);
    _scene->render();
}

void GameController::touchesBegan(double x, double y, long long t_ms) {
    _begin_touch_time = t_ms;
    _invalid_touch = _in_purchase || _ui_transition_running || _store_handler->isTransitionRunning();
    _touches_begin_point.x = x;
    _touches_begin_point.y = _height - y;
    _is_tap = !_invalid_touch;

    if (_invalid_touch)
        return;

    if (inInteractiveView()) {
        _scene->touchesBegan(_touches_begin_point, t_ms);
    }
}

void GameController::touchesMoved(double x, double y, long long t_ms) {
    if (_invalid_touch)
        return;

    const vec2<double> q(x, _height - y);
    if (fabs(q.x - _touches_begin_point.x) > ui_button_size ||
        fabs(q.y - _touches_begin_point.y) > ui_button_size) {
        _is_tap = false;
    }
    if (inInteractiveView()) {
        _scene->touchesMoved(q, t_ms);
    }
}

void GameController::touchesEnded(double x, double y, long long t_ms) {
    if (_invalid_touch)
        return;

    const vec2<double> q(x, _height - y);
    if (fabs(q.x - _touches_begin_point.x) > ui_button_size ||
        fabs(q.y - _touches_begin_point.y) > ui_button_size ||
        t_ms - _begin_touch_time > ms_for_1s) {
        _is_tap = false;
    }
    if (inInteractiveView()) {
        _scene->touchesEnded(q, t_ms);
    }

    if (_is_tap) {
        tapped({static_cast<float>(_touches_begin_point.x), static_cast<float>(_touches_begin_point.y)});
    } else if (_current_view == IViewDelegate<HomeView>::view) {
        _singleplayer_handler->startGame();
        showHighscoreView();
    }
}

void GameController::didRetrieveRank(unsigned long long rank, unsigned short index) {
    const auto type = static_cast<leaderboard_type>(index);
    IViewDelegate<MenuView>::view->setLeaderboardRank(rank, type);
    if (type == leaderboard_type::all_time) {
        IViewDelegate<GameOverView>::view->setLeaderboardRank(rank);
    }
}

void GameController::didRetrievePurchase(const char *price, in_app_purchase_type type) {
    const auto wstr = loader->wstr(price);
    _store_handler->setPrice(type, wstr);
    IViewDelegate<HeartsView>::view->setHeartPrice(wstr);
}

void GameController::didBuyInAppPurchase(in_app_purchase_type type) {
    _in_purchase = false;
    setPaused(false);
    if (type == in_app_purchase_type_hearts) {
        setHeartButtonEnabled(false);
        _state.base_state.max_continued_game_count = 2;
        save();

        _singleplayer_handler->setHeartTargetAlpha(0.f, false);
        if (_background_view) {
            startTransition(_background_view);
        } else if (_current_view == IViewDelegate<MenuView>::view) {
            if (_singleplayer_handler->gameStarted()) {
                showHighscoreView();
            } else {
                startTransition(IViewDelegate<HomeView>::view);
            }
        }
    }
}

void GameController::didCancelInAppPurchase() {
    _in_purchase = false;
    setPaused(false);
}

void GameController::didTapBackButton() {
    _back_button_tapped = true;
}

//void GameController::didTapRateAppButton(bool rated) {
//    if (rated) {
//        _state.rate_app_prompts_count = _user_rated_already;
//        _app_delegate->save(_state);
//    } else if (_state.rate_app_prompts_count != _user_rated_already) {
//        _state.rate_app_prompts_count++;
//        _app_delegate->save(_state);
//    }
//}

void GameController::setAuthenticated(bool authenticated) {
    if (authenticated) {
        IViewDelegate<MenuView>::view->setAuthenticated(true);
        IViewDelegate<GameOverView>::view->setAuthenticated(true);
        _app_delegate->getLeaderboardData();
        _achievements_handler->setAuthenticated(true);
    } else {
        IViewDelegate<MenuView>::view->setAuthenticated(false);
        IViewDelegate<GameOverView>::view->setAuthenticated(false);
        _achievements_handler->setAuthenticated(false);
    }
}

void GameController::didReceiveAudioInterruptNotification(bool start) {
    if (_al_renderer) {
        if (start) {
            _al_renderer->pause();
            _al_renderer->freeContext();
        } else {
            _al_renderer->resume();
        }
    }
}

// MARK: - ISingleplayerHandlerDelegate

void GameController::setHighscore(unsigned long long score, bool post_to_leaderboard) {
    IViewDelegate<HomeView>::view->setHighscore(score);
    IViewDelegate<MenuView>::view->setHighscore(score);
    if (post_to_leaderboard) {
        _app_delegate->setLeaderboardData(score);
    }
}

void GameController::setScore(unsigned long long score) {
    IViewDelegate<HighscoreView>::view->setScore(score);
}

void GameController::setShotScore(unsigned long long score) {
    IViewDelegate<HighscoreView>::view->setShotScore(score);
}

void GameController::setNotification(skill_shot_type type) {
    if (_singleplayer_handler->state().game_type == game_type::highscore) {
        IViewDelegate<HighscoreView>::view->setNotification(type);
    } else {
        IViewDelegate<GoalsView>::view->setNotification(type);
    }
}

void GameController::setNotificationAlpha(float alpha) {
    if (_singleplayer_handler->state().game_type == game_type::highscore) {
        IViewDelegate<HighscoreView>::view->setNotificationAlpha(alpha);
    } else {
        IViewDelegate<GoalsView>::view->setNotificationAlpha(alpha);
    }
}

void GameController::setUpgradeNotificationAlpha(float alpha) {
    IViewDelegate<GameOverView>::view->setNotificationAlpha(alpha);
}

void GameController::setBarInfoAlpha(float alpha) {
    IViewDelegate<HighscoreView>::view->setBarInfoAlpha(alpha);
}

void GameController::setButtonInfoAlpha(float alpha) {
    IViewDelegate<HighscoreView>::view->setButtonInfoAlpha(alpha);
    IViewDelegate<GoalsView>::view->setButtonInfoAlpha(alpha);
}

void GameController::setSuperchargerAlpha(float alpha) {
    IViewDelegate<HighscoreView>::view->setSuperchargerAlpha(alpha);
}

void GameController::addChipProgress(unsigned long long progress) {
    const auto type = _state.base_state.chip_type;
    const auto level = _store_handler->currentLevel(type);
    _store_handler->addProgress(progress);
    IViewDelegate<GameOverView>::view->setChipProgress(_store_handler->progress(type), _store_handler->pointsRequired(type));

    const auto new_level = _store_handler->currentLevel(type);
    if (new_level > level) {
        if (type == chip_type_default && new_level == max_chip_level) {
            IViewDelegate<HomeView>::view->setGoalsAlpha(1.f);
        }
        _achievements_handler->chipUpgraded(type, new_level);
        IViewDelegate<GameOverView>::view->setChipType(type, new_level);
        IViewDelegate<GameOverView>::view->setNotification(type, level);
        _singleplayer_handler->showUpgradeNotification();
        _al_renderer->playSound<sound_type::goal_completed>();
    }
}

void GameController::setProgress(unsigned long long progress, unsigned long long count) {
    IViewDelegate<GoalsView>::view->setProgress(progress, count);
}

void GameController::setGoal(unsigned short goal) {
    if (goal < goals_count) {
        IViewDelegate<GoalsView>::view->setGoal(goal);
    } else {
        IViewDelegate<HomeView>::view->setGoalsAlpha(ui_disabled_alpha);
    }
    IViewDelegate<HomeView>::view->setGoals(goal, _store_handler->goalsToUnlockNextChip());
    setMaxContinuedGameCount();
}

void GameController::setVideoType(video_type type) {
    if (type == video_type_range) {
        IViewDelegate<GoalsView>::view->setVideoButtonEnabled(false);
    } else {
        IViewDelegate<GoalsView>::view->setVideoButtonEnabled(true);
        IViewDelegate<VideoView>::view->setNotification(type);
        _video_handler->setVideoType(type);
    }
}

void GameController::showHomeView() {
    startTransition(IViewDelegate<HomeView>::view);
}

void GameController::showHighscoreView() {
    startTransition(IViewDelegate<HighscoreView>::view);
}

void GameController::showGoalCompletedView() {
    const auto goal = _singleplayer_handler->state().goal;
    _al_renderer->playSound<sound_type::goal_completed>();
    _store_handler->goalCompleted(goal);
    IViewDelegate<GoalCompletedView>::view->setProgress(goal, _store_handler->goalsToUnlockNextChip());
    _achievements_handler->goalCompleted(_store_handler->unlockedChip());
    startTransition(IViewDelegate<GoalCompletedView>::view);
}

void GameController::showGameOverView(unsigned long long score, bool new_highscore_reached) {
    const auto type = _state.base_state.chip_type;
    IViewDelegate<GameOverView>::view->setChipType(type, _store_handler->currentLevel(type));
    IViewDelegate<GameOverView>::view->setChipProgress(_store_handler->progress(type), _store_handler->pointsRequired(type));
    IViewDelegate<GameOverView>::view->setScore(score, new_highscore_reached);
    IViewDelegate<GameOverView>::view->setAuthenticated(_app_delegate->authenticatedForLeaderboard());
    _app_delegate->setLeaderboardData(score);
    startTransition(IViewDelegate<GameOverView>::view);
}

void GameController::showIntro(intro_type type) {
    IViewDelegate<AlertView>::view->setTitle(type, _state.base_state.bounds.origin.y);
    startTransition(IViewDelegate<AlertView>::view);
}

void GameController::updateHeartsAlpha() {
    setHeartsAlpha(1.f);
}

void GameController::setBackgroundData(const float gradient[6], float blur_intensity) {
    _gl_renderer->setBackgroundGradient(gradient);
    IViewDelegate<StoreView>::view->setBlurIntensity(blur_intensity);
}

void GameController::setChipType(chip_type type, unsigned short level, float progress) {
    IViewDelegate<StoreView>::view->setChipType(type, level, progress);
}

void GameController::setGoalsProgress(unsigned short total) {
    IViewDelegate<StoreView>::view->setGoalsProgress(_singleplayer_handler->state().goal, total);
}

void GameController::setIndicatorLevel(unsigned short index, unsigned short level) {
    IViewDelegate<StoreView>::view->setIndicatorLevel(index, level);
}

void GameController::setIndicatorAlpha(unsigned short index, float alpha) {
    IViewDelegate<StoreView>::view->setIndicatorAlpha(index, alpha);
}

void GameController::setViewAlpha(float alpha) {
    IViewDelegate<StoreView>::view->setViewAlpha(alpha);
}

void GameController::didChangePage() {
    _al_renderer->playSound<sound_type::chip_passed>();
}

void GameController::setCancelable(bool cancelable) {
    IViewDelegate<TutorialView>::view->setCancelable(cancelable);
}

void GameController::setTutorialScore(unsigned long long score) {
    IViewDelegate<TutorialView>::view->setScore(score);
}

void GameController::setTutorialShotScore(unsigned long long score) {
    IViewDelegate<TutorialView>::view->setShotScore(score);
}

void GameController::setTutorialTitle(tutorial_type type) {
    IViewDelegate<TutorialView>::view->setNotification(type);
}

void GameController::setTutorialTitleAlpha(float alpha) {
    IViewDelegate<TutorialView>::view->setNotificationAlpha(alpha);
}

void GameController::exitTutorial() {
    achievements_handler->tutorialCompleted();
    _singleplayer_handler->showCue();
    _scene->setGameHandler(_singleplayer_handler);
    startTransition(IViewDelegate<HomeView>::view);
}

void GameController::showAlert(tutorial_type type) {
    if (type < tutorial_type_range - 1) {
        _al_renderer->playSound(type);
    } else {
        _al_renderer->playSound<sound_type::goal_completed>();
    }
    IViewDelegate<AlertView>::view->setTitle(type);
    startTransition(IViewDelegate<AlertView>::view);
}

void GameController::achievementUnlocked(achievement_type type) {
    IViewDelegate<MenuView>::view->setAchievements(_achievements_handler->achieved(), achievement_type_range);
}

void GameController::setAchievementsProgress(const float progress[achievement_type_range]) {
    _app_delegate->setAchievementsProgress(progress);
}

// MARK: - private

void GameController::setPaused(bool paused) {
    if (paused) {
        _al_renderer->pause();
        _gl_renderer->pause();
    } else {
        _al_renderer->resume();
    }
    _app_delegate->setPaused(paused);
}

void GameController::save() {
    _app_delegate->save(_state);
    _store_handler->save();
    _singleplayer_handler->save();
    _tutorial_handler->save();
    _achievements_handler->save();
}

void GameController::startTransition(IView *next_view) {
    if (_ui_transition_running && _current_view == next_view) {
        _ui_transition_progress = 1.f - _ui_transition_progress;
        _current_view = _next_view;
        _next_view = next_view;
    } else if (_current_view != next_view) {
        _ui_transition_running = true;
        _ui_transition_progress = 0.f;

        if (_background_view) {
            if (next_view->type() == view_type::full_screen) {
                if (_background_view == next_view) {
                    _background_view = _current_view;
                    _current_view = next_view;
                    _next_view = nullptr;
                    _background_view_transition_data = {1.f, 0.f};
                    _current_view_transition_data = {0.8f, -0.8f};
                    _transition_coeff = 2.f;
                } else {
                    _next_view = next_view;
                    _background_view_transition_data = {-0.4f, -0.8f};
                    _current_view_transition_data = {2.f, 0.f};
                    _transition_coeff = 1.f;
                }
            } else {
                _next_view = next_view;
                _background_view_transition_data = {0.f, -0.8f};
                _current_view_transition_data = {2.f, 0.f};
                _transition_coeff = 1.f;
            }
        } else {
            if (next_view->type() == view_type::full_screen) {
                _next_view = next_view;
                _background_view_transition_data = {0.f, 0.f};
                _current_view_transition_data = {2.f, 0.f};
                _transition_coeff = 1.f;
            } else {
                _background_view = _current_view;
                _current_view = next_view;
                _next_view = nullptr;
                _scene->setAlertView(_current_view);
                _background_view_transition_data = {0.8f, 0.f};
                _current_view_transition_data = {1.f, -1.f};
                _transition_coeff = 2.f;
            }
        }
    }
}

void GameController::setTransitionProgress(IView *view, const vec2<float> &transition_data) {
    if (view) {
        const auto progress = transition_data.x * _ui_transition_progress + transition_data.y;
        view->setTransitionProgress(progress);
        if (inGameView(view)) {
            if (_state.base_state.chip_type != chip_type_default) {
                _scene->setChargeAlpha(1.f - fabs(progress));
            }
            setHeartsAlpha(1.f - fabs(progress));
        }
    }
}

void GameController::setMaxContinuedGameCount() {
    if (_singleplayer_handler->state().goal >= goals_for_hearts) {
        _state.base_state.max_continued_game_count = 2;
    } else if (!hasHearts()) {
        _state.base_state.max_continued_game_count = 0;
    }
}

void GameController::setHeartsAlpha(float alpha) {
    if (hasHearts()) {
        _scene->setHeartAlpha(0, alpha);
        _scene->setHeartAlpha(1, _singleplayer_handler->state().continued_game_count > 1 ? alpha * ui_disabled_alpha : alpha);
        _scene->setHeartAlpha(2, _singleplayer_handler->state().continued_game_count > 0 ? alpha * ui_disabled_alpha : alpha);
    }
}

void GameController::setHeartButtonEnabled(bool enabled) {
    IViewDelegate<HighscoreView>::view->setHeartButtonEnabled(enabled);
    IViewDelegate<GoalsView>::view->setHeartButtonEnabled(enabled);
}

void GameController::showMenuView(bool in_game) {
    if (_app_delegate->authenticatedForLeaderboard()) {
        _app_delegate->getLeaderboardData();
        IViewDelegate<MenuView>::view->setAuthenticated(true);
    } else {
        IViewDelegate<MenuView>::view->setAuthenticated(false);
    }
    if (in_game) {
        IViewDelegate<MenuView>::view->setScore(_singleplayer_handler->state().score);
        IViewDelegate<MenuView>::view->setInGame(true);
    } else {
        IViewDelegate<MenuView>::view->setInGame(false);
    }
    startTransition(IViewDelegate<MenuView>::view);
}

void GameController::showStoreView() {
    _state.discard_base_state = true;
    _store_transition_data = {2.f, -1.f};
    _scene->setGameHandler(_store_handler);
    return startTransition(IViewDelegate<StoreView>::view);
}

void GameController::share() {
    const auto score = _singleplayer_handler->lastScore();
    std::string str;
    if (_singleplayer_handler->state().highscore != score) {
        const auto type = loader->localizedString(loader->key(_state.base_state.chip_type).c_str());
        str = loader->str(loader->localizedString("share_score", score, type));
    } else {
        str = loader->str(loader->localizedString("share_highscore", score));
    }
    _app_delegate->share(str.c_str());
}

void GameController::tapped(HomeView *view, const vec2<float> &p) {
    if (view->tapped<button_type_store>(p)) {
        showStoreView();
    } else if (view->tapped<button_type_menu>(p)) {
        showMenuView(false);
    } else if (view->tapped<button_type_goals>(p)) {
        startTransition(IViewDelegate<GoalsView>::view);
    }
}

void GameController::tapped(HighscoreView *view, const vec2<float> &p) {
    if (_scene->chipsMoving() || _singleplayer_handler->chipSelected(p))
        return;

    if (view->tapped<button_type_menu>(p)) {
        showMenuView(true);
    } else if (view->tapped<button_type_heart>(p)) {
        _singleplayer_handler->setHeartTargetAlpha(0.f);
        startTransition(IViewDelegate<HeartsView>::view);
    } else if (view->tapped<button_type_supercharger>(p)) {
        _singleplayer_handler->supercharge();
    }
}

void GameController::tapped(GoalsView *view, const vec2<float> &p) {
    if (_scene->chipsMoving() || _singleplayer_handler->chipSelected(p))
        return;

    if (view->tapped<button_type_ad>(p)) {
        _state.discard_base_state = true;
        _video_transition_data = {2.f, -1.f};
        _scene->setGameHandler(_video_handler);
        _scene->setBackgroundBlending(false);
        startTransition(IViewDelegate<VideoView>::view);
    } else if (view->tapped<button_type_cross>(p)) {
        startTransition(IViewDelegate<HomeView>::view);
    } else if (view->tapped<button_type_heart>(p)) {
        _singleplayer_handler->setHeartTargetAlpha(0.f);
        startTransition(IViewDelegate<HeartsView>::view);
    }
}

void GameController::tapped(GoalCompletedView *view, const vec2<float> &p) {
    if (view->tapped<button_type_tick>(p)) {
        const auto unlocked_chip = _store_handler->unlockedChip();
        const auto goal = _singleplayer_handler->state().goal;
        if (!hasHearts() && goal >= goals_for_hearts_purchase && !_hearts_info_shown) {
            _hearts_info_shown = true;
            setHeartButtonEnabled(true);
            setButtonInfoAlpha(1.f);
            _singleplayer_handler->setHeartTargetAlpha(1.f);
        }
        if (unlocked_chip < chip_type_range) {
            _singleplayer_handler->resetGame();
            IViewDelegate<UnlockedView>::view->setChip(unlocked_chip);
            _state.base_state.waiting_for_user = true;
            startTransition(IViewDelegate<UnlockedView>::view);
        } else if (goal == goals_count) {
            _singleplayer_handler->resetGame();
            IViewDelegate<UnlockedView>::view->setSuperchargerUnlocked();
            _state.base_state.waiting_for_user = true;
            startTransition(IViewDelegate<UnlockedView>::view);
        } else if (goal == goals_for_hearts && !hasHearts()) {
            _singleplayer_handler->resetGame();
            IViewDelegate<UnlockedView>::view->setHeartsUnlocked();
            _state.base_state.waiting_for_user = true;
            startTransition(IViewDelegate<UnlockedView>::view);
        } else {
            setGoal(goal);
            _singleplayer_handler->resetGame();
            _singleplayer_handler->startGame();
            startTransition(IViewDelegate<GoalsView>::view);
        }
    }
}

void GameController::tapped(UnlockedView *view, const vec2<float> &p) {
    if (view->tapped<button_type_store>(p)) {
        setGoal(_singleplayer_handler->state().goal);
        _state.base_state.waiting_for_user = false;
        showStoreView();
    } else if (view->tapped<button_type_tick>(p)) {
        const auto goal = _singleplayer_handler->state().goal;
        setGoal(goal);
        _state.base_state.waiting_for_user = false;
        _state.discard_base_state = true;
        _singleplayer_handler->showCue();
        if (goal == goals_for_hearts) {
            setHeartButtonEnabled(false);
            _singleplayer_handler->startGame();
            startTransition(IViewDelegate<GoalsView>::view);
        } else {
            _singleplayer_handler->setGameType(game_type::highscore);
            startTransition(IViewDelegate<HomeView>::view);
        }
    }
}

void GameController::tapped(TutorialView *view, const vec2<float> &p) {
    if (view->tapped<button_type_cross>(p)) {
        _tutorial_handler->cancelTutorial();
    }
}

void GameController::tapped(AlertView *view, const vec2<float> &p) {
    if (view->tapped<button_type_tick>(p)) {
        if (view->introShowing()) {
            _singleplayer_handler->setIntroSeen();
            startTransition(_background_view);
        } else {
            _tutorial_handler->showNextTutorial();
            startTransition(IViewDelegate<TutorialView>::view);
        }
    }
}

void GameController::tapped(MenuView *view, const vec2<float> &p) {
    if (view->tapped<button_type_cross>(p)) {
        if (_singleplayer_handler->gameStarted()) {
            showHighscoreView();
        } else {
            startTransition(IViewDelegate<HomeView>::view);
        }
    } else if (view->tapped<button_type_tutorial>(p)) {
        _scene->setGameHandler(_tutorial_handler);
        _tutorial_handler->showNextTutorial();
        startTransition(IViewDelegate<TutorialView>::view);
    } else if (view->tapped<button_type_exit>(p)) {
        _singleplayer_handler->endGame();
    } else if (view->tapped<button_type_mute>(p)) {
        _state.muted = true;
        _al_renderer->mute();
        view->setMuted(_state.muted);
    } else if (view->tapped<button_type_unmute>(p)) {
        _state.muted = false;
        _al_renderer->unmute();
        view->setMuted(_state.muted);
    } else if (view->tapped<button_type_sign_in>(p) || view->leaderboardTapped(p)) {
        _app_delegate->showLeaderboard();
    } else if (view->achievementsTapped(p)) {
        _app_delegate->showAchievements();
    } else if (view->tapped<button_type_download>(p)) {
        _app_delegate->restorePurchases();
    }
}

void GameController::tapped(GameOverView *view, const vec2<float> &p) {
    if (view->tapped<button_type_tick>(p)) {
        _singleplayer_handler->resetGame();
        startTransition(IViewDelegate<HomeView>::view);
    } else if (view->tapped<button_type_sign_in>(p)) {
        _app_delegate->showLeaderboard();
    } else if (view->tapped<button_type_share>(p)) {
        share();
    }
}

void GameController::tapped(HeartsView *view, const vec2<float> &p) {
    if (view->tapped<button_type_cross>(p)) {
        startTransition(_background_view);
    } else if (view->buyButtonTapped(p)) {
        startPurchase(in_app_purchase_type_hearts);
    }
}

void GameController::tapped(TwitterView *view, const vec2<float> &p) {
    if (view->tapped<button_type_cross>(p)) {
        startTransition(IViewDelegate<HomeView>::view);
    } else if (view->iconTapped(p)) {
        _app_delegate->showTwitterWebsite();
        _state.followed_on_twitter = true;
        _app_delegate->save(_state);
        startTransition(IViewDelegate<HomeView>::view);
    }
}

void GameController::tapped(StoreView *view, const vec2<float> &p) {
    if (view->tapped<button_type_tick>(p) || view->tapped<button_type_cross>(p)) {
        _store_transition_data = {-2.f, 1.f};
        _store_handler->startExitTransition();
        _singleplayer_handler->setGameType(game_type::highscore);
        startTransition(IViewDelegate<HomeView>::view);
    }
}

void GameController::tapped(VideoView *view, const vec2<float> &p) {
    if (view->tapped<button_type_cross>(p)) {
        _video_transition_data = {-2.f, 1.f};
        _video_handler->startExitTransition();
        startTransition(IViewDelegate<GoalsView>::view);
    }
}

void GameController::tapped(const vec2<float> &p) {
    if (!_ui_transition_running) {
        _current_view->tapped(p);
    }
}

void GameController::startPurchase(in_app_purchase_type type) {
    if (type < in_app_purchase_type_range) {
        _in_purchase = true;
        setPaused(true);
        _app_delegate->startPurchase(type);
    }
}

bool GameController::shouldAskToRate() {
    if (_state.rate_app_prompts_count == _user_rated_already)
        return false;

    return _state.rate_app_prompts_count && _state.rate_app_prompts_count <= _max_consecutive_rate_prompts;
}

bool GameController::inGameView(IView *view) const {
    return view == IViewDelegate<HighscoreView>::view || view == IViewDelegate<GoalsView>::view;
}

bool GameController::hasHearts() const {
    return _state.base_state.max_continued_game_count == 2;
}

bool GameController::inInteractiveView() const {
    return inGameView(_current_view) || _current_view == IViewDelegate<HomeView>::view ||
        _current_view == IViewDelegate<TutorialView>::view || _current_view == IViewDelegate<StoreView>::view;
}
