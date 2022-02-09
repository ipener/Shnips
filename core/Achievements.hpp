// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ACHIEVEMENT_HPP
#define ACHIEVEMENT_HPP

#include "achievement_type.h"
#include "app_state.hpp"
#include "skill_shot_type.h"

class IAchievementDelegate {
public:
    virtual void achievementUnlocked(achievement_type type) = 0;
};

class IAchievement {
public:
    IAchievement(unsigned long count);

    virtual void unlocked() = 0;

    virtual bool achieved() const;

    virtual float progress() const;

    void setProgress(unsigned long *progress);

    void addProgress(unsigned long progress = 1);

public:
    static IAchievementDelegate          *delegate;
    static const ::base_state            *base_state;
    static const app_state::singleplayer *state;

protected:
    unsigned long *_progress = nullptr;
    unsigned long  _count;
};

template<achievement_type T>
class TAchievement : public IAchievement {
public:
    using IAchievement::IAchievement;

protected:
    void unlocked() override {
        IAchievement::delegate->achievementUnlocked(T);
    }
};

class TutorialCompletedAchievement : public TAchievement<achievement_type_tutorial_completed> {
public:
    using TAchievement<achievement_type_tutorial_completed>::TAchievement;

    void tutorialCompleted();
};

class SuccessfulShotsAchievement : public TAchievement<achievement_type_successful_shots> {
public:
    using TAchievement<achievement_type_successful_shots>::TAchievement;

    void submit();
};

class CushionShotsAchievement : public TAchievement<achievement_type_cushion_shots> {
public:
    using TAchievement<achievement_type_cushion_shots>::TAchievement;

    void submit();
};

class NoCushionShotsAchievement : public TAchievement<achievement_type_no_cushion_shots> {
public:
    using TAchievement<achievement_type_no_cushion_shots>::TAchievement;

    void submit();
};

class ConsecutiveShotsAchievement : public TAchievement<achievement_type_consecutive_shots> {
public:
    using TAchievement<achievement_type_consecutive_shots>::TAchievement;

    float progress() const override;

    void submit();
};

class ConsecutiveFastShotsAchievement : public TAchievement<achievement_type_consecutive_fast_shots> {
public:
    using TAchievement<achievement_type_consecutive_fast_shots>::TAchievement;

    float progress() const override;
    
    void submit();

    void beginShot();

    void endShot();

private:
    unsigned short _shots = 0;
    long long      _begin_shot_time = 0;
    long long      _end_shot_time = 0;
};

template<chip_type C, achievement_type T>
class ChipUnlockedAchievement : public TAchievement<T> {
public:
    using TAchievement<T>::TAchievement;

    void chipUnlocked(chip_type type);
};

class ChipTypeFastUnlockedAchievement : public ChipUnlockedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked> {
public:
    using ChipUnlockedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked>::ChipUnlockedAchievement;
};

class ChipTypeExtraPointsUnlockedAchievement : public ChipUnlockedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked> {
public:
    using ChipUnlockedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked>::ChipUnlockedAchievement;
};

class ChipTypeSwapUnlockedAchievement : public ChipUnlockedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked> {
public:
    using ChipUnlockedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked>::ChipUnlockedAchievement;
};

class ChipTypeExtraChipUnlockedAchievement : public ChipUnlockedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked> {
public:
    using ChipUnlockedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked>::ChipUnlockedAchievement;
};

class GoalsCompletedAchievement : public TAchievement<achievement_type_tutorial_completed> {
public:
    using TAchievement<achievement_type_tutorial_completed>::TAchievement;

    void goalCompleted();
};

class PlayedEveryDayAchievement : public TAchievement<achievement_type_played_every_day> {
public:
    using TAchievement<achievement_type_played_every_day>::TAchievement;

    bool achieved() const override;

    float progress() const override;

    void submit();

private:
    void dateChanged(int day, int month, unsigned short num_days);
};

// _count must be < 2^4
class HalfScreenPlayAchievement : public TAchievement<achievement_type_half_screen_play> {
public:
    using TAchievement<achievement_type_half_screen_play>::TAchievement;

    bool achieved() const override;

    float progress() const override;

    void update();

    void submit();

private:
    enum screen_half : unsigned char {
        left,
        right,
        top,
        bottom
    };

    unsigned short progress(screen_half half) const;

private:
    unsigned char _last_shot_mask = 0x0F; // 4 screen halves; "1111" means all valid.
};

class LoseAt666Achievement : public TAchievement<achievement_type_lose_at_666> {
public:
    using TAchievement<achievement_type_lose_at_666>::TAchievement;

    bool achieved() const override;

    float progress() const override;

    void submit();
};

// 9 taps ... _ _ _ ...
class SOSAchievement : public TAchievement<achievement_type_sos> {
public:
    using TAchievement<achievement_type_sos>::TAchievement;

    void touchesBegan(long long t_ms);

    void touchesEnded(long long t_ms);

private:
    const unsigned short _long_signal_max_t = 4000;
    const unsigned short _long_signal_min_t = 600;
    const unsigned short _short_signal_max_t = 300;
    long long            _begin_touch_time = -1;
    long long            _end_touch_time = -1;
    unsigned short       _steps = 0;
    unsigned short       _long_signal_mask = 0;
    unsigned short       _short_signal_mask = 0;
    unsigned short       _needed_long_signal_mask = 56; // 111000 in binary
    unsigned short       _needed_short_signal_mask = 455; // 111000111 in binary
};

class AllChipsTouchingCushionsAchievement : public TAchievement<achievement_type_all_chips_touching_cushions> {
public:
    using TAchievement<achievement_type_all_chips_touching_cushions>::TAchievement;

    void submit();

private:
    const double _max_delta = 5.0;
};

class SmileyAchievement : public TAchievement<achievement_type_smiley> {
public:
    using TAchievement<achievement_type_smiley>::TAchievement;

    void touchesBegan(const vec2<double> &p);

    void touchesEnded(const vec2<double> &p);

    void touchesMoved(const vec2<double> &p);

private:
    bool isSmile(const vec2<double> &p);

private:
    const double _max_tap_distance = 15.f;

    vec2<double> _eyes[3];
    bool         _is_eye[3];
    vec2<double> _points_sum;
    long long    _points_count = 0;
    double       _max_y;
    short        _current_eye = 0;
};

class AllSkillShotsAchievedAchievement : public TAchievement<achievement_type_all_skill_shots_achieved> {
public:
    using TAchievement<achievement_type_all_skill_shots_achieved>::TAchievement;

    bool achieved() const override;

    float progress() const override;

    void skillShotAchieved(skill_shot_type type);
};

class HeartsEarnedAchievement : public TAchievement<achievement_type_hearts_earned> {
public:
    using TAchievement<achievement_type_hearts_earned>::TAchievement;

    void goalCompleted();
};

class HeartsUsedAchievement : public TAchievement<achievement_type_hearts_used> {
public:
    using TAchievement<achievement_type_hearts_used>::TAchievement;
};

class SuperchargedAchievement : public TAchievement<achievement_type_supercharged> {
public:
    using TAchievement<achievement_type_supercharged>::TAchievement;
};

template<chip_type C, achievement_type T>
class ChipUpgradedAchievement : public TAchievement<T> {
public:
    using TAchievement<T>::TAchievement;

    void chipUpgraded(chip_type type, unsigned short level);
};

class ChipTypeFastUpgradedAchievement : public ChipUpgradedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked> {
public:
    using ChipUpgradedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked>::ChipUpgradedAchievement;
};

class ChipTypeExtraPointsUpgradedAchievement : public ChipUpgradedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked> {
public:
    using ChipUpgradedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked>::ChipUpgradedAchievement;
};

class ChipTypeSwapUpgradedAchievement : public ChipUpgradedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked> {
public:
    using ChipUpgradedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked>::ChipUpgradedAchievement;
};

class ChipTypeExtraChipUpgradedAchievement : public ChipUpgradedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked> {
public:
    using ChipUpgradedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked>::ChipUpgradedAchievement;
};

class ScorePointsAchievement : public TAchievement<achievement_type_score_points> {
public:
    using TAchievement<achievement_type_score_points>::TAchievement;

    void submit();
};

#endif
