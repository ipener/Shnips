// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ACHIEVEMENTSHANDLER_HPP
#define ACHIEVEMENTSHANDLER_HPP

#include "Achievements.hpp"
#include "assets.hpp"
#include "IAchievementsHandler.hpp"

#define __HIDDEN

class AchievementsHandler : public IAchievementsHandler, public IAchievementDelegate {
public:
    AchievementsHandler(const ::base_state *base_state, const app_state::singleplayer *state, class IStateHandler *handler);

    // MARK: - IAchievementsHandler
    void tutorialCompleted() override;

    void submit() override;

    void beginShot() override;

    void endShot() override;

    void update() override;

    void heartUsed() override;

    void goalCompleted(chip_type unlocked) override;

    void skillShotAchieved(skill_shot_type) override;

    void supercharged() override;

    void chipUpgraded(chip_type type, unsigned short level) override;

    void touchesBegan(const vec2<double> &p, long long t_ms) override;

    void touchesMoved(const vec2<double> &p, long long t_ms) override;

    void touchesEnded(const vec2<double> &p, long long t_ms) override;

    // MARK: - IAchievementDelegate
    void achievementUnlocked(achievement_type type) override;

    // MARK: -
    void init(class IAchievementsHandlerDelegate *delegate);

    void setAuthenticated(bool authenticated);

    void save();

    unsigned short achieved() const;

private:
    void updateProgress();

private:
    class IStateHandler                   *_state_handler = nullptr;
    class IAchievementsHandlerDelegate    *_delegate = nullptr;

    TutorialCompletedAchievement           _tutorial_completed {1};
    SuccessfulShotsAchievement             _successful_shots {1000};
    CushionShotsAchievement                _cushion_shots {1000};
    NoCushionShotsAchievement              _no_cushion_shots {500};
    ConsecutiveShotsAchievement            _consecutive_shots {100};
    ConsecutiveFastShotsAchievement        _consecutive_fast_shots {20};
    ChipTypeFastUnlockedAchievement        _chip_type_fast_unlocked {1};
    ChipTypeExtraPointsUnlockedAchievement _chip_type_extra_points_unlocked {1};
    ChipTypeSwapUnlockedAchievement        _chip_type_swap_unlocked {1};
    ChipTypeExtraChipUnlockedAchievement   _chip_type_extra_chip_unlocked {1};
    GoalsCompletedAchievement              _goals_completed {assets::goals_count};
    PlayedEveryDayAchievement              _played_every_day {30};
    HalfScreenPlayAchievement              _half_screen_play {5};
    LoseAt666Achievement                   _lose_at_666 {1}; __HIDDEN
    SOSAchievement                         _sos {1}; __HIDDEN
    AllChipsTouchingCushionsAchievement    _all_chips_touching_cushions {1};
    SmileyAchievement                      _smiley {1}; __HIDDEN
    AllSkillShotsAchievedAchievement       _all_skill_shots_achieved {10};
    HeartsEarnedAchievement                _hearts_earned {assets::goals_for_hearts};
    HeartsUsedAchievement                  _hearts_used {100};
    SuperchargedAchievement                _supercharged {100}; __HIDDEN
    ChipTypeFastUpgradedAchievement        _chip_type_fast_upgraded {3};
    ChipTypeExtraPointsUpgradedAchievement _chip_type_extra_points_upgraded {3};
    ChipTypeSwapUpgradedAchievement        _chip_type_swap_upgraded {3};
    ChipTypeExtraChipUpgradedAchievement   _chip_type_extra_chip_upgraded {3};
    ScorePointsAchievement                 _score_points {1000000};

    app_state::achievements                _state;
    std::vector<float>                     _current_progress;
    std::vector<float>                     _last_progress;
    bool                                   _authenticated = false;
};

#undef __HIDDEN
#endif
