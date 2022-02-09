// Copyright © 2018 Igor Pener. All rights reserved.

#include "AchievementsHandler.hpp"

#include <chrono>
#include "IAchievementsHandlerDelegate.hpp"
#include "IStateHandler.hpp"
#include "Logger.hpp"

IAchievementsHandler *achievements_handler = nullptr;

AchievementsHandler::AchievementsHandler(const ::base_state *base_state, const app_state::singleplayer *state, IStateHandler *handler) : _state_handler(handler) {
    IAchievement::delegate = this;
    IAchievement::base_state = base_state;
    IAchievement::state = state;
}

void AchievementsHandler::tutorialCompleted() {
    if (!_tutorial_completed.achieved()) {
        _tutorial_completed.tutorialCompleted();
    }
}

void AchievementsHandler::submit() {
    if (!_successful_shots.achieved()) {
        _successful_shots.submit();
    }
    if (!_cushion_shots.achieved()) {
        _cushion_shots.submit();
    }
    if (!_no_cushion_shots.achieved()) {
        _no_cushion_shots.submit();
    }
    if (!_consecutive_shots.achieved()) {
        _consecutive_shots.submit();
    }
    if (!_consecutive_fast_shots.achieved()) {
        _consecutive_fast_shots.submit();
    }
    if (!_played_every_day.achieved()) {
        _played_every_day.submit();
    }
    if (!_half_screen_play.achieved()) {
        _half_screen_play.submit();
    }
    if (!_lose_at_666.achieved()) {
        _lose_at_666.submit();
    }
    if (!_all_chips_touching_cushions.achieved()) {
        _all_chips_touching_cushions.submit();
    }
    if (!_score_points.achieved()) {
        _score_points.submit();
    }
    if (_authenticated) {
        updateProgress();
    }
}

void AchievementsHandler::beginShot() {
    if (!_consecutive_fast_shots.achieved()) {
        _consecutive_fast_shots.beginShot();
    }
}

void AchievementsHandler::endShot() {
    if (!_consecutive_fast_shots.achieved()) {
        _consecutive_fast_shots.endShot();
    }
}

void AchievementsHandler::update() {
    if (!_half_screen_play.achieved()) {
        _half_screen_play.update();
    }
}

void AchievementsHandler::heartUsed() {
    if (!_hearts_used.achieved()) {
        _hearts_used.addProgress(1);
    }
}

void AchievementsHandler::goalCompleted(chip_type unlocked) {
    if (!_goals_completed.achieved()) {
        _goals_completed.goalCompleted();
    }
    if (!_hearts_earned.achieved()) {
        _hearts_earned.goalCompleted();
    }
    if (!_chip_type_fast_unlocked.achieved()) {
        _chip_type_fast_unlocked.chipUnlocked(unlocked);
    }
    if (!_chip_type_extra_points_unlocked.achieved()) {
        _chip_type_extra_points_unlocked.chipUnlocked(unlocked);
    }
    if (!_chip_type_swap_unlocked.achieved()) {
        _chip_type_swap_unlocked.chipUnlocked(unlocked);
    }
    if (!_chip_type_extra_chip_unlocked.achieved()) {
        _chip_type_extra_chip_unlocked.chipUnlocked(unlocked);
    }
    chipUpgraded(unlocked, 1);
}

void AchievementsHandler::skillShotAchieved(skill_shot_type type) {
    if (!_all_skill_shots_achieved.achieved()) {
        _all_skill_shots_achieved.skillShotAchieved(type);
    }
}

void AchievementsHandler::supercharged() {
    if (!_supercharged.achieved()) {
        _supercharged.addProgress();
    }
}

void AchievementsHandler::chipUpgraded(chip_type type, unsigned short level) {
    if (!_chip_type_fast_upgraded.achieved()) {
        _chip_type_fast_upgraded.chipUpgraded(type, level);
    }
    if (!_chip_type_extra_points_upgraded.achieved()) {
        _chip_type_extra_points_upgraded.chipUpgraded(type, level);
    }
    if (!_chip_type_swap_upgraded.achieved()) {
        _chip_type_swap_upgraded.chipUpgraded(type, level);
    }
    if (!_chip_type_extra_chip_upgraded.achieved()) {
        _chip_type_extra_chip_upgraded.chipUpgraded(type, level);
    }
}

void AchievementsHandler::touchesBegan(const vec2<double> &p, long long t_ms) {
    if (!_smiley.achieved()) {
        _smiley.touchesBegan(p);
    }
    if (!_sos.achieved()) {
        _sos.touchesBegan(t_ms);
    }
}

void AchievementsHandler::touchesMoved(const vec2<double> &p, long long t_ms) {
    if (!_smiley.achieved()) {
        _smiley.touchesMoved(p);
    }
}

void AchievementsHandler::touchesEnded(const vec2<double> &p, long long t_ms) {
    if (!_smiley.achieved()) {
        _smiley.touchesEnded(p);
    }
    if (!_sos.achieved()) {
        _sos.touchesEnded(t_ms);
    }
}

void AchievementsHandler::achievementUnlocked(achievement_type type) {
    save();
    _delegate->achievementUnlocked(type);
    if (_authenticated) {
        updateProgress();
    }
}

void AchievementsHandler::init(IAchievementsHandlerDelegate *delegate) {
    _delegate = delegate;

    if (!_state_handler->load(&_state)) {
        logi << "AchievementsHandler failed reading state" << std::endl;
        _state.progress.clear();
    }
    for (unsigned short i = _state.progress.size(); i < achievement_type_range; ++i) {
        _state.progress.push_back(0);
    }

    _tutorial_completed.setProgress(&_state.progress[achievement_type_tutorial_completed]);
    _successful_shots.setProgress(&_state.progress[achievement_type_successful_shots]);
    _cushion_shots.setProgress(&_state.progress[achievement_type_cushion_shots]);
    _no_cushion_shots.setProgress(&_state.progress[achievement_type_no_cushion_shots]);
    _consecutive_shots.setProgress(&_state.progress[achievement_type_consecutive_shots]);
    _consecutive_fast_shots.setProgress(&_state.progress[achievement_type_consecutive_fast_shots]);
    _chip_type_fast_unlocked.setProgress(&_state.progress[achievement_type_chip_type_fast_unlocked]);
    _chip_type_extra_points_unlocked.setProgress(&_state.progress[achievement_type_chip_type_extra_points_unlocked]);
    _chip_type_swap_unlocked.setProgress(&_state.progress[achievement_type_chip_type_swap_unlocked]);
    _chip_type_extra_chip_unlocked.setProgress(&_state.progress[achievement_type_chip_type_extra_chip_unlocked]);
    _goals_completed.setProgress(&_state.progress[achievement_type_goals_completed]);
    _played_every_day.setProgress(&_state.progress[achievement_type_played_every_day]);
    _half_screen_play.setProgress(&_state.progress[achievement_type_half_screen_play]);
    _lose_at_666.setProgress(&_state.progress[achievement_type_lose_at_666]);
    _sos.setProgress(&_state.progress[achievement_type_sos]);
    _all_chips_touching_cushions.setProgress(&_state.progress[achievement_type_all_chips_touching_cushions]);
    _smiley.setProgress(&_state.progress[achievement_type_smiley]);
    _all_skill_shots_achieved.setProgress(&_state.progress[achievement_type_all_skill_shots_achieved]);
    _hearts_earned.setProgress(&_state.progress[achievement_type_hearts_earned]);
    _hearts_used.setProgress(&_state.progress[achievement_type_hearts_used]);
    _supercharged.setProgress(&_state.progress[achievement_type_supercharged]);
    _chip_type_fast_upgraded.setProgress(&_state.progress[achievement_type_chip_type_fast_upgraded]);
    _chip_type_extra_points_upgraded.setProgress(&_state.progress[achievement_type_chip_type_extra_points_upgraded]);
    _chip_type_swap_upgraded.setProgress(&_state.progress[achievement_type_chip_type_swap_upgraded]);
    _chip_type_extra_chip_upgraded.setProgress(&_state.progress[achievement_type_chip_type_extra_chip_upgraded]);
    _score_points.setProgress(&_state.progress[achievement_type_score_points]);

    _last_progress.resize(_state.progress.size(), 0.f);
    _current_progress.resize(_state.progress.size(), 0.f);
}

void AchievementsHandler::setAuthenticated(bool authenticated) {
    if (_authenticated == authenticated)
        return;

    _authenticated = authenticated;
    if (_authenticated) {
        updateProgress();
    } else {
        fill(_last_progress.begin(), _last_progress.end(), 0.f);
    }
}

void AchievementsHandler::save() {
    _state_handler->save(_state);
}

unsigned short AchievementsHandler::achieved() const {
    return (
        _tutorial_completed.achieved() +
        _successful_shots.achieved() +
        _cushion_shots.achieved() +
        _no_cushion_shots.achieved() +
        _consecutive_shots.achieved() +
        _consecutive_fast_shots.achieved() +
        _chip_type_fast_unlocked.achieved() +
        _chip_type_extra_points_unlocked.achieved() +
        _chip_type_swap_unlocked.achieved() +
        _chip_type_extra_chip_unlocked.achieved() +
        _goals_completed.achieved() +
        _played_every_day.achieved() +
        _half_screen_play.achieved() +
        _lose_at_666.achieved() +
        _sos.achieved() +
        _all_chips_touching_cushions.achieved() +
        _smiley.achieved() +
        _all_skill_shots_achieved.achieved() +
        _hearts_earned.achieved() +
        _hearts_used.achieved() +
        _supercharged.achieved() +
        _chip_type_fast_upgraded.achieved() +
        _chip_type_extra_points_upgraded.achieved() +
        _chip_type_swap_upgraded.achieved() +
        _chip_type_extra_chip_upgraded.achieved() +
        _score_points.achieved()
    );
}

void AchievementsHandler::updateProgress() {
    _last_progress = _current_progress;

    _current_progress[achievement_type_tutorial_completed] = _tutorial_completed.progress();
    _current_progress[achievement_type_successful_shots] = _successful_shots.progress();
    _current_progress[achievement_type_cushion_shots] = _cushion_shots.progress();
    _current_progress[achievement_type_no_cushion_shots] = _no_cushion_shots.progress();
    _current_progress[achievement_type_consecutive_shots] = _consecutive_shots.progress();
    _current_progress[achievement_type_consecutive_fast_shots] = _consecutive_fast_shots.progress();
    _current_progress[achievement_type_chip_type_fast_unlocked] = _chip_type_fast_unlocked.progress();
    _current_progress[achievement_type_chip_type_extra_points_unlocked] = _chip_type_extra_points_unlocked.progress();
    _current_progress[achievement_type_chip_type_swap_unlocked] = _chip_type_swap_unlocked.progress();
    _current_progress[achievement_type_chip_type_extra_chip_unlocked] = _chip_type_extra_chip_unlocked.progress();
    _current_progress[achievement_type_goals_completed] = _goals_completed.progress();
    _current_progress[achievement_type_played_every_day] = _played_every_day.progress();
    _current_progress[achievement_type_half_screen_play] = _half_screen_play.progress();
    _current_progress[achievement_type_lose_at_666] = _lose_at_666.progress();
    _current_progress[achievement_type_sos] = _sos.progress();
    _current_progress[achievement_type_all_chips_touching_cushions] = _all_chips_touching_cushions.progress();
    _current_progress[achievement_type_smiley] = _smiley.progress();
    _current_progress[achievement_type_all_skill_shots_achieved] = _all_skill_shots_achieved.progress();
    _current_progress[achievement_type_hearts_earned] = _hearts_earned.progress();
    _current_progress[achievement_type_hearts_used] = _hearts_used.progress();
    _current_progress[achievement_type_supercharged] = _supercharged.progress();
    _current_progress[achievement_type_chip_type_fast_upgraded] = _chip_type_fast_upgraded.progress();
    _current_progress[achievement_type_chip_type_extra_points_upgraded] = _chip_type_extra_points_upgraded.progress();
    _current_progress[achievement_type_chip_type_swap_upgraded] = _chip_type_swap_upgraded.progress();
    _current_progress[achievement_type_chip_type_extra_chip_upgraded] = _chip_type_extra_chip_upgraded.progress();
    _current_progress[achievement_type_score_points] = _score_points.progress();

    float progress[achievement_type_range];
    bool progress_changed = false;

    for (unsigned short i = 0; i < achievement_type_range; ++i) {
        if (_current_progress[i] - _last_progress[i] > 1e-5f) {
            progress[i] = _current_progress[i];
            progress_changed = true;
        } else {
            progress[i] = 0.f;
        }
    }
    if (progress_changed > 1e-5f) {
        _delegate->setAchievementsProgress(progress);
    }
}
