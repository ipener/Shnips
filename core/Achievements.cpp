// Copyright © 2018 Igor Pener. All rights reserved.

#include "Achievements.hpp"

#include <chrono>
#include "assets.hpp"
#include "Logger.hpp"
#include "utils.hpp"

using namespace std::chrono;
using namespace assets;

IAchievementDelegate *IAchievement::delegate = nullptr;
const ::base_state *IAchievement::base_state = nullptr;
const app_state::singleplayer *IAchievement::state = nullptr;

IAchievement::IAchievement(unsigned long count) : _count(count) {
}

bool IAchievement::achieved() const {
    return _progress && *_progress >= _count;
}

float IAchievement::progress() const {
    return _progress ? *_progress / static_cast<float>(_count) : 0.f;
}

void IAchievement::setProgress(unsigned long *progress) {
    _progress = progress;
}

void IAchievement::addProgress(unsigned long progress) {
    *_progress += progress;
    if (*_progress >= _count) {
        *_progress = _count;
        unlocked();
    }
}

void TutorialCompletedAchievement::tutorialCompleted() {
    addProgress();
}

void SuccessfulShotsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        addProgress();
    }
}

void CushionShotsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes &&
        IAchievement::base_state->shot_data.max_cushions_before_pass) {
        addProgress();
    }
}

void NoCushionShotsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes &&
        IAchievement::base_state->shot_data.max_cushions_before_pass == 0 &&
        IAchievement::base_state->shot_data.cushions_after_last_pass == 0) {
        addProgress();
    }
}

float ConsecutiveShotsAchievement::progress() const {
    return _progress && *_progress >= _count ? 1.f : 0.f;
}

void ConsecutiveShotsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        addProgress();
    } else {
        *_progress = 0;
    }
}

float ConsecutiveFastShotsAchievement::progress() const {
    return _progress && *_progress >= _count ? 1.f : 0.f;
}

void ConsecutiveFastShotsAchievement::beginShot() {
    _begin_shot_time = utils::currentTimestampMs();
    _end_shot_time = 0;
}

void ConsecutiveFastShotsAchievement::endShot() {
    if (_end_shot_time)
        return;

    _end_shot_time = utils::currentTimestampMs();
}

void ConsecutiveFastShotsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes && (_end_shot_time - _begin_shot_time <= ms_for_1s)) {
        ++_shots;
        (*_progress) = _shots;
        if ((*_progress) >= _count) {
            (*_progress) = _count;
            unlocked();
        }
    } else {
        _shots = 0;
        (*_progress) = 0;
    }
}

template<chip_type C, achievement_type T>
void ChipUnlockedAchievement<C, T>::chipUnlocked(chip_type type) {
    if (type == C) {
        IAchievement::addProgress();
    }
}

void GoalsCompletedAchievement::goalCompleted() {
    *_progress = IAchievement::state->goal;
    if (achieved()) {
        unlocked();
    }
}

bool PlayedEveryDayAchievement::achieved() const {
    return _progress && *_progress % 100 >= _count;
}

float PlayedEveryDayAchievement::progress() const {
    return achieved() ? 1.f : 0.f;
}

void PlayedEveryDayAchievement::submit() {
    system_clock::time_point now = system_clock::now();
    time_t tt = system_clock::to_time_t(now);
    time_t yesterday = system_clock::to_time_t(now - hours(24));
    tm utc_today = *gmtime(&tt);
    tm utc_yesterday = *gmtime(&yesterday);

    int last_day = static_cast<int>(*_progress / 10000);
    int last_month = static_cast<int>((*_progress / 100) % 100);
    int day_count = static_cast<int>(*_progress % 100);

    if (utc_yesterday.tm_mon == last_month && utc_yesterday.tm_mday == last_day) {
        day_count++;
        dateChanged(utc_today.tm_mday, utc_today.tm_mon, day_count);
        if (day_count >= _count) {
            unlocked();
        }
    } else if (utc_today.tm_mon != last_month || utc_today.tm_mday != last_day) {
        day_count = 1;
        dateChanged(utc_today.tm_mday, utc_today.tm_mon, day_count);
    }
}

void PlayedEveryDayAchievement::dateChanged(int day, int month, unsigned short num_days) {
    *_progress = static_cast<unsigned long>(day * 10000 + month * 100 + num_days);
}

void HalfScreenPlayAchievement::update() {
    if (_last_shot_mask) {
        for (const auto &chip : IAchievement::base_state->chips_data) {
            if (chip.p.x + chip.r - 1e-6 > screen_size.x / 2.0) {
                _last_shot_mask &= ~(1 << left);
            } else if (chip.p.x - chip.r + 1e-6 < screen_size.x / 2.0) {
                _last_shot_mask &= ~(1 << right);
            }
            if (chip.p.y + chip.r - 1e-6 > screen_size.y / 2.0) {
                _last_shot_mask &= ~(1 << top);
            } else if (chip.p.y - chip.r + 1e-6 < screen_size.y / 2.0) {
                _last_shot_mask &= ~(1 << bottom);
            }
        }
    }
}

unsigned short HalfScreenPlayAchievement::progress(screen_half half) const {
    return static_cast<unsigned short>((*_progress & (0x0F << (half * 4))) >> (half * 4));
}

bool HalfScreenPlayAchievement::achieved() const {
    return progress(top) >= _count || progress(bottom) >= _count || progress(left) >= _count || progress(right) >= _count;
}

float HalfScreenPlayAchievement::progress() const {
    return achieved() ? 1.f : 0.f;
}

void HalfScreenPlayAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        for (int i = 0; i < 4; ++i) {
            screen_half half = static_cast<screen_half>(i);
            if (_last_shot_mask & (1 << half)) {
                *_progress += 1 << (half * 4);
                if (progress(half) >= _count) {
                    unlocked();
                    break;
                }
            } else {
                *_progress &= ~(0x0F << (half * 4));
            }
        }
    } else {
        *_progress = 0;
    }
    _last_shot_mask = 15;
}

bool LoseAt666Achievement::achieved() const {
    return *_progress <= 666 ? false : true;
}

float LoseAt666Achievement::progress() const {
    return achieved() ? 1.f : 0.f;
}

void LoseAt666Achievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        *_progress = static_cast<unsigned long>(IAchievement::state->score);
        if (*_progress > 666) {
            *_progress = 0;
        }
    } else if (*_progress == 666) {
        *_progress = 667;
        unlocked();
    } else {
        *_progress = 0;
    }
}

void SOSAchievement::touchesBegan(long long t_ms) {
    _begin_touch_time = t_ms;
}

void SOSAchievement::touchesEnded(long long t_ms) {
    _end_touch_time = t_ms;
    long long diff = _end_touch_time - _begin_touch_time;

    if (_steps == 9) {
        _short_signal_mask >>= 1;
        _long_signal_mask >>= 1;
        _steps--;
    }
    if (diff < _short_signal_max_t) {
        _short_signal_mask |= (1<<_steps);
        _long_signal_mask &= ~(1<<_steps);
    } else if (diff > _long_signal_min_t && diff < _long_signal_max_t) {
        _long_signal_mask |= (1<<_steps);
        _short_signal_mask &= ~(1<<_steps);
    }
    _steps++;
    if (_steps == 9) {
        if (_long_signal_mask == _needed_long_signal_mask && _short_signal_mask == _needed_short_signal_mask) {
            addProgress();
        }
    }
}

void AllChipsTouchingCushionsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        for (const auto &chip : IAchievement::base_state->chips_data) {
            if (chip.p.x - chip.r - 1e-6 > _max_delta &&
                chip.p.x + chip.r + 1e-6 < screen_size.x - _max_delta &&
                chip.p.y - chip.r - 1e-6 > _max_delta &&
                chip.p.y + chip.r + 1e-6 < screen_size.y - _max_delta) {
                return;
            }
        }
        addProgress();
    }
}

void SmileyAchievement::touchesEnded(const vec2<double> &p) {
    _max_y = std::max(_max_y, p.y);
    if (fabs(_eyes[_current_eye].x - p.x) > _max_tap_distance ||
        fabs(_eyes[_current_eye].y - p.y) > _max_tap_distance) {
        // not an eye.
        _is_eye[_current_eye] = false;
    }
    _points_sum += p;
    _points_count++;
    _points_sum /= _points_count;

    if (isSmile(p)) {
        addProgress();
    } else if (_current_eye == 2) {
        _eyes[0] = _eyes[1];
        _eyes[1] = _eyes[2];
        _is_eye[0] = _is_eye[1];
        _is_eye[1] = _is_eye[2];
    } else {
        _current_eye++;
    }
}

void SmileyAchievement::touchesMoved(const vec2<double> &p) {
    _max_y = std::max(_max_y, p.y);
    if (fabs(_eyes[_current_eye].x - p.x) > _max_tap_distance ||
        fabs(_eyes[_current_eye].y - p.y) > _max_tap_distance) {
        // not an eye.
        _is_eye[_current_eye] = false;
    }

    if (_points_count < 1000) {
        _points_sum += p;
        _points_count++;
    }
}

void SmileyAchievement::touchesBegan(const vec2<double> &p) {
    _max_y = p.y;
    _is_eye[_current_eye] = true;
    _eyes[_current_eye] = p;
    _points_sum = p;
    _points_count = 1;
}

bool SmileyAchievement::isSmile(const vec2<double> &p) {
    return _current_eye > 1 && _is_eye[0] && _is_eye[1] &&
        fabs(_eyes[1].y - _eyes[0].y) < 50 && fabs(_eyes[0].x - _eyes[1].x) >= 50 &&
        _points_count > 2 && _max_y + 20 <= _eyes[0].y && _max_y + 20 <= _eyes[1].y &&
        p.dist(_eyes[2]) >= 50;
}

bool AllSkillShotsAchievedAchievement::achieved() const {
    return utils::countBits(*_progress) == skill_shot_type_range;
}

float AllSkillShotsAchievedAchievement::progress() const {
    return _progress ? utils::countBits(*_progress) / static_cast<float>(skill_shot_type_range) : 0;
}

void AllSkillShotsAchievedAchievement::skillShotAchieved(skill_shot_type type) {
    *_progress |= 1 << type;
    if (achieved()) {
        unlocked();
    }
}

void HeartsEarnedAchievement::goalCompleted() {
    *_progress = IAchievement::state->goal;
    if (achieved()) {
        unlocked();
    }
}

template<chip_type C, achievement_type T>
void ChipUpgradedAchievement<C, T>::chipUpgraded(chip_type type, unsigned short level) {
    if (type == C) {
        if (level == 1) {
            *TAchievement<T>::_progress = 1;
        } else if (level == 2) {
            *TAchievement<T>::_progress = 2;
        } else if (level == 3) {
            *TAchievement<T>::_progress = 3;
            TAchievement<T>::unlocked();
        }
    }
}

void ScorePointsAchievement::submit() {
    if (IAchievement::base_state->shot_data.passes) {
        addProgress(static_cast<unsigned long>(IAchievement::state->score));
    }
}

template class ChipUnlockedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked>;
template class ChipUnlockedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked>;
template class ChipUnlockedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked>;
template class ChipUnlockedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked>;
template class ChipUpgradedAchievement<chip_type_fast, achievement_type_chip_type_fast_unlocked>;
template class ChipUpgradedAchievement<chip_type_extra_points, achievement_type_chip_type_extra_points_unlocked>;
template class ChipUpgradedAchievement<chip_type_swap, achievement_type_chip_type_swap_unlocked>;
template class ChipUpgradedAchievement<chip_type_extra_chip, achievement_type_chip_type_extra_chip_unlocked>;
