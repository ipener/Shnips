// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SINGLEPLAYERHANDLER_HPP
#define SINGLEPLAYERHANDLER_HPP

#include <queue>
#include "assets.hpp"
#include "IGameHandler.hpp"
#include "IGoalsHandlerDelegate.hpp"
#include "replay_state.hpp"
#include "SkillShotHandler.hpp"

class SingleplayerHandler : public GameStateHandler<app_state::singleplayer>, public ISkillShotHandlerDelegate, public IGoalsHandlerDelegate {
public:
    SingleplayerHandler(::base_state *base_state, IStateHandler *state_handler);

    ~SingleplayerHandler();

    void init(class ISingleplayerHandlerDelegate *delegate, class ISingleplayerSceneDelegate *scene_delegate);

    void onChipsRestored() override;

    void willResignActive() override;

    void willBecomeActive() override;

    bool collisionEnabled() const override;

    bool interactionAllowed() const override;

    bool playableGame() const override;

    void submit() override;

    void passed(unsigned short i, unsigned short j) override;

    void hit(unsigned short i) override;

    void chipHit(unsigned short i) override;

    void cushionHit() override;

    void update(float t) override;

    void touchesBegan(const vec2<double> &p, long long t_ms) override;

    void touchesEnded(const vec2<double> &p, long long t_ms) override;

    bool chipSelected(const vec2<float> p);

    void setGameType(game_type type);

    void setHeartTargetAlpha(float alpha, bool animated = true);

    void showCue();

    void showUpgradeNotification();

    void startScoreAnimation();

    void startGame();

    void endGame();

    void resetGame();

    void setIntroSeen();

    void supercharge();

    bool gameStarted() const;

    unsigned long long lastScore() const;

private:
    // MARK: - ISkillShotHandlerDelegate
    void skillShotAchieved(skill_shot_type type) override;

    void setNotification(skill_shot_type type) override;

    // MARK: - IGoalsHandlerDelegate
    void goalCompleted(unsigned short id) override;

    void setProgress(unsigned long long progress) override;

    void setVideoType(video_type type) override;

    // MARK: -
    void updateHighscore(bool post_to_leaderboard = false);

    void updateScore(unsigned short &score);

    void addScore(unsigned short score);

    void resetCue();

    void checkExtraChipNeeded();

    void chargeChips(float charge);

    void dischargeChips();

    bool chipsCharged() const;

    float superchargerAlpha() const;

    float defaultAbilityTime() const;

    void continueHelperShotComputation();

    bool cuePathCollidesWithChips(double x, double y) const;

    bool cueOutsideBounds(double x, double y, const rect<double> &bounds) const;

    void checkIndicatorCueAround0Degrees();

    void setIndicatorCue(short position);

private:
    static const unsigned short        _max_360_degree_iterations = 360;

    const double                       _subangle = 360.0 / static_cast<double>(_max_360_degree_iterations);
    const float                        _default_level_1_ability_time = 15.f;
    const float                        _default_level_2_ability_time = 25.f;
    const float                        _faster_charging_coeff = 1.25f;
    const float                        _fast_level_1_speed_coeff = 1.5f;
    const float                        _fast_level_2_speed_coeff = 2.f;
    const float                        _discharge_coeff = 0.018f;
    const float                        _score_charge_reduction_coeff = 22.f;
    const unsigned short               _min_consecutive_angle_iterations = 22 * _max_360_degree_iterations / 360;

    class GoalsHandler                 *_goals_handler = nullptr;
    class ISingleplayerHandlerDelegate *_delegate = nullptr;
    class ISingleplayerSceneDelegate   *_scene_delegate = nullptr;
    SkillShotHandler                   _skill_shot_handler;

    replay_state                       _cue_animation_state;
    disc                               _indicator_cue;

    unsigned long long                 _last_score = 0;
    float                              _heart_target_alpha = 0.f;
    float                              _heart_alpha = 0.f;
    float                              _supercharger_target_alpha = 0.f;
    float                              _supercharger_alpha = 0.f;
    float                              _charge_progress = 0.f;
    float                              _notification_animation_progress = 0.f;
    float                              _info_display_time_left = 0.f;
    unsigned short                     _shot_score = 0;
    unsigned short                     _current_angle_attempt = 0;
    unsigned short                     _current_chip_shot_attempt = 0;
    unsigned short                     _start_consecutive_successful_attempts_cnt = 0;
    unsigned short                     _end_consecutive_successful_attempts_cnt = 0;
    unsigned short                     _widest_valid_shot_angle_found = 0;
    bool                               _valid_shot_angle[_max_360_degree_iterations];
    bool                               _first_pass = true;
    bool                               _cue_animation_running = false;
    bool                               _score_animation_running = false;
    bool                               _notification_animation_running = false;
    bool                               _info_animation_running = false;
    bool                               _missed = false;
    bool                               _ability_used = false;
    bool                               _touching_field = false;
    bool                               _charging_enabled = true;
};

#endif
