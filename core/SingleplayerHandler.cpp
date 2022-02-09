// Copyright © 2018 Igor Pener. All rights reserved.

#include "SingleplayerHandler.hpp"

#include "IAchievementsHandler.hpp"
#include "GoalsHandler.hpp"
#include "ISingleplayerHandlerDelegate.hpp"
#include "ISingleplayerSceneDelegate.hpp"
#include "Logger.hpp"
#include "SkillShotHandler.hpp"

using namespace std;
using namespace assets;

SingleplayerHandler::SingleplayerHandler(::base_state *base_state, IStateHandler *state_handler) : GameStateHandler<app_state::singleplayer>(base_state, state_handler) {
    _goals_handler = new GoalsHandler(base_state, &_state, this);
}

SingleplayerHandler::~SingleplayerHandler() {
    delete _goals_handler;
}

void SingleplayerHandler::init(ISingleplayerHandlerDelegate *delegate, ISingleplayerSceneDelegate *scene_delegate) {
    _delegate = delegate;
    _scene_delegate = scene_delegate;

    if (!load()) {
        logi << "SingleplayerHandler failed reading state" << endl;
    }
    for (const auto &chip : _base_state->chips_data) {
        if (chip.v != 0.0) {
            _hit = true;
            break;
        }
    }

    _goals_handler->setCurrentGoal(_state.goal);
    bool reset_chips = false;
    if (_state.game_type == game_type::goals) {
        if (_state.progress == 0) {
            reset_chips = true;
            _state.score = 0;
            _state.game_type = game_type::highscore;
        }
    }
    _base_state->bounds.size.x = screen_size.x;
    setGameType(_state.game_type);

    _skill_shot_handler.init(_base_state, this);
    if (_base_state->waiting_for_user) {
        updateHighscore(true);
        _state.score = 0;
        _base_state->waiting_for_user = false;
        dischargeChips();
        IGameHandler::submit();
        achievements_handler->submit();
        _goals_handler->submit();
        reset_chips = true;
    }
    if (!gameStarted()) {
        reset_chips = true;
    }
    if (reset_chips) {
        _base_state->last_index = 0xFFFF;
        int i = 0;
        Chip::defaultChipPositions(default_chips_count, _base_state->bounds,
            [this, &i](const vec2<double> &p) {
                _base_state->chips_data[i].p = p;
                _base_state->chips_data[i].v = {};
                _base_state->chips_data[i].icon = glyphs::chips[_base_state->chips_data[i].type];
                ++i;
            }
        );
    }
    if (_base_state->ability_time_left > 1e-5f) {
        _scene_delegate->setChargedAnimation(true);
    }
    _skill_shot_handler.setEnabled(!chipsCharged());
    _goals_handler->beginShot();

    _indicator_cue.r = Chip::default_size;
    _delegate->setScore(_state.score);
    _delegate->setHighscore(_state.highscore, false);

    Chip::defaultShotState(_cue_animation_state);
}

void SingleplayerHandler::onChipsRestored() {
    _skill_shot_handler.setEnabled(true);
    if (_base_state->last_index < default_chips_count) {
        const auto type = _base_state->chips_data[_base_state->last_index].type;
        _base_state->chips_data[_base_state->last_index].icon = glyphs::chips[type];
    }
    _base_state->last_index = 0xFFFF;
    resetCue();
}

void SingleplayerHandler::willResignActive() {
    if (_base_state->ability_time_left > 1e-5f) {
        _scene_delegate->setChargedAnimation(false);
        if (_base_state->chip_type == chip_type_fast) {
            _scene_delegate->setChipsSpeedCoefficient(1.f);
        }
    }
}

void SingleplayerHandler::willBecomeActive() {
    if (_base_state->ability_time_left > 1e-5f) {
        _scene_delegate->setChargedAnimation(true);
        if (_base_state->chip_type == chip_type_fast) {
            if (_base_state->chips_data[0].level > 2) {
                _scene_delegate->setChipsSpeedCoefficient(_fast_level_2_speed_coeff);
            } else {
                _scene_delegate->setChipsSpeedCoefficient(_fast_level_1_speed_coeff);
            }
        }
    }
}

bool SingleplayerHandler::collisionEnabled() const {
    return _touching_field || _hit;
}

bool SingleplayerHandler::interactionAllowed() const {
    return !_base_state->waiting_for_user;
}

bool SingleplayerHandler::playableGame() const {
    return true;
}

void SingleplayerHandler::submit() {
    IGameHandler::submit();
    _skill_shot_handler.submit();
    _ability_used = false;

    if (_base_state->chips_data.size() > default_chips_count) {
        _scene_delegate->removeExtraChips();
    }
    if (!_base_state->max_continued_game_count && _state.score > 10) {
        _heart_target_alpha = 0.f;
    }
    if (_base_state->shot_data.passes) {
        _supercharger_target_alpha = superchargerAlpha();

        if (!_state.charging_intro_seen && _base_state->chip_type != chip_type_default) {
            _charging_enabled = false;
            _delegate->showIntro(intro_type_charging);
        }
        achievements_handler->submit();
        _goals_handler->submit();
        if (_state.game_type == game_type::highscore) {
            _delegate->setScore(_state.score);
        }
        checkExtraChipNeeded();
    } else {
        _missed = true;
        if (_state.score > 0) {
            if (_state.continued_game_count < _base_state->max_continued_game_count) {
                _info_display_time_left = 0.f;
                _missed = false;
                _state.continued_game_count++;
                _supercharger_target_alpha = superchargerAlpha();
                achievements_handler->heartUsed();
            } else {
                endGame();
            }
            _delegate->updateHeartsAlpha();
        } else {
            if (_state.game_type == game_type::highscore) {
                _delegate->showHomeView();
                updateHighscore();
            }
            resetGame();
        }
    }

    _goals_handler->beginShot();

    _base_state->shot_data.passes = 0;
    _base_state->shot_data.max_cushions_before_pass = 0;
    _base_state->shot_data.cushions_after_last_pass = 0;
    _shot_score = 0;

    _current_angle_attempt = 0;
    _current_chip_shot_attempt = 0;
    _start_consecutive_successful_attempts_cnt = 0;
    _end_consecutive_successful_attempts_cnt = 0;
    _first_pass = true;
    _widest_valid_shot_angle_found = 0;

    _skill_shot_handler.setEnabled(!chipsCharged());
}

void SingleplayerHandler::passed(unsigned short i, unsigned short j) {
    _base_state->shot_data.passes++;
    _base_state->shot_data.max_cushions_before_pass = max(
        _base_state->shot_data.max_cushions_before_pass,
        _base_state->shot_data.cushions_after_last_pass
    );

    unsigned short score = 1;
    score += _base_state->shot_data.cushions_after_last_pass;

    updateScore(score);
    addScore(score);
    _base_state->shot_data.cushions_after_last_pass = 0;
    _skill_shot_handler.passed(i, j);
}

void SingleplayerHandler::hit(unsigned short i) {
    if (i != 0xFFFF) {
        if (_base_state->chip_type == chip_type_default) {
            _supercharger_target_alpha = 0.f;
        } else if (_state.goal >= goals_count) {
            _supercharger_target_alpha = ui_disabled_alpha;
        }
        _delegate->setBarInfoAlpha(0.f);
        _cue_animation_running = _touching_field = false;
        IGameHandler::hit(i);
        _skill_shot_handler.hit();
        if (chipsCharged() || _base_state->chips_data.size() > default_chips_count) {
            _ability_used = true;
        }
        _goals_handler->endShot();
    } else if (!_info_animation_running) {
        _info_display_time_left = info_display_time_s;
        _info_animation_running = true;
    }
}

void SingleplayerHandler::chipHit(unsigned short i) {
    _skill_shot_handler.chipHit(i);
}

void SingleplayerHandler::cushionHit() {
    _base_state->shot_data.cushions_after_last_pass++;
    _skill_shot_handler.cushionHit();
}

void SingleplayerHandler::update(float t) {
    _skill_shot_handler.update(t);
    chargeChips(-t);

    if (_cue_animation_running && !_touching_field) {
        _base_state->cue_animation_time_s += t;

        if (_base_state->cue_animation_time_s + 1e-5f > cue_animation_duration_s) {
            resetCue();
        } else if (_base_state->cue_animation_time_s > touch_hide_time_s &&
                   _base_state->cue_animation_time_s < cue_hide_time_s && !_hit) {
            _base_state->cue.p = _indicator_cue.p + _indicator_cue.v * (_base_state->cue_animation_time_s - touch_hide_time_s) / (cue_hide_time_s - touch_hide_time_s);
        } else if (_base_state->cue_animation_time_s > cue_hide_time_s) {
            _base_state->cue_visible = false;
        }
    }
    if (_score_animation_running) {
        const auto progress = _state.score > 200 ? 2 : 1;
        if (_state.score < progress) {
            _score_animation_running = false;
            if (_state.score) {
                _delegate->addChipProgress(_state.score);
            }
        } else {
            _state.score -= progress;
            _delegate->addChipProgress(progress);
        }
    }
    if (_info_animation_running) {
        _info_display_time_left -= t;
        if (_info_display_time_left < 1e-5f) {
            _info_display_time_left = 0.f;
            _delegate->setBarInfoAlpha(0.f);
            _info_animation_running = false;
        } else if (_info_display_time_left < 0.5f) {
            _delegate->setBarInfoAlpha(2.f * _info_display_time_left);
        } else if (_info_display_time_left > info_display_time_s - 0.5f) {
            _delegate->setBarInfoAlpha(2.f * (info_display_time_s - _info_display_time_left));
        } else {
            _delegate->setBarInfoAlpha(1.f);
        }
    }
    if (_notification_animation_running) {
        _notification_animation_progress += ui_transition_step * t;
        if (_notification_animation_progress > 1.f) {
            _notification_animation_progress = 1.f;
            _notification_animation_running = false;
        }
        _delegate->setUpgradeNotificationAlpha(_notification_animation_progress);
    }
    _delegate->setNotificationAlpha(_skill_shot_handler.notificationAlpha());

    bool supercharger_animation_running = false;
    if (_supercharger_alpha - _supercharger_target_alpha < -1e-5f) {
        _supercharger_alpha += ui_transition_step * t;
        if (_supercharger_alpha > _supercharger_target_alpha) {
            _supercharger_alpha = _supercharger_target_alpha;
        }
        supercharger_animation_running = true;
    } else if (_supercharger_alpha - _supercharger_target_alpha > 1e-5f) {
        _supercharger_alpha -= ui_transition_step * t;
        if (_supercharger_alpha < _supercharger_target_alpha) {
            _supercharger_alpha = _supercharger_target_alpha;
        }
        supercharger_animation_running = true;
    }
    if (supercharger_animation_running) {
        if (_state.supercharge_count == 0) {
            const auto alpha = max(0.f, (_supercharger_alpha - ui_disabled_alpha) / (1.f - ui_disabled_alpha));
            _delegate->setSuperchargerAlpha(_supercharger_alpha);
            _delegate->setButtonInfoAlpha(alpha);
        } else {
            _delegate->setSuperchargerAlpha(_supercharger_alpha);
            _delegate->setButtonInfoAlpha(0.f);
        }
    }
    bool heart_animation_running = false;
    if (_heart_alpha - _heart_target_alpha < -1e-5f) {
        _heart_alpha += ui_transition_step * t;
        if (_heart_alpha > _heart_target_alpha) {
            _heart_alpha = _heart_target_alpha;
        }
        heart_animation_running = true;
    } else if (_heart_alpha - _heart_target_alpha > 1e-5f) {
        _heart_alpha -= ui_transition_step * t;
        if (_heart_alpha < _heart_target_alpha) {
            _heart_alpha = _heart_target_alpha;
        }
        heart_animation_running = true;
    }
    if (heart_animation_running) {
        _delegate->setButtonInfoAlpha(_heart_alpha);
    }

    _scene_delegate->setChargeProgress(_charge_progress);

    if (!_missed && !_cue_animation_running && !collisionEnabled() && _state.score > 0 &&
        _base_state->chips_data.size() == default_chips_count) {
        continueHelperShotComputation();
    }
}

void SingleplayerHandler::touchesBegan(const vec2<double> &p, long long t_ms) {
    _touching_field = _base_state->cue_visible = true;
    _base_state->cue_animation_time_s = touch_hide_time_s;
}

void SingleplayerHandler::touchesEnded(const vec2<double> &p, long long t_ms) {
    _touching_field = false;
    _base_state->cue_visible = false;

    if (_cue_animation_running) {
        resetCue();
    }
}

bool SingleplayerHandler::chipSelected(const vec2<float> p) {
    if (_base_state->chip_type != chip_type_swap || !chipsCharged())
        return false;

    const auto size = _base_state->chips_data.size();
    _base_state->cue.p.x = p.x;
    _base_state->cue.p.y = p.y;
    _base_state->cue.v = {};
    for (size_t i = 0; i < size; ++i) {
        if (_base_state->last_index != i && _base_state->chips_data[i].collides(_base_state->cue)) {
            if (_base_state->last_index < default_chips_count) {
                const auto type = _base_state->chips_data[_base_state->last_index].type;
                _base_state->chips_data[_base_state->last_index].icon = glyphs::chips[type];
            }

            _base_state->last_index = i;
            _base_state->chips_data[_base_state->last_index].icon = glyphs::blank_chip;
            _ability_used = true;
            _touching_field = false;
            _goals_handler->swappedChips();
            return true;
        }
    }
    return false;
}

void SingleplayerHandler::setGameType(game_type type) {
    _state.game_type = type;
    if (_state.game_type == game_type::goals) {
        _base_state->bounds.origin.y = 2.f * ui_paragraph_margin;
        _base_state->bounds.size.y = screen_size.y - _base_state->bounds.origin.y;
        _delegate->setGoal(_state.goal);
        _goals_handler->setEnabled(true);
    } else {
        _base_state->bounds.origin.y = 0.0;
        _base_state->bounds.size.y = screen_size.y;
        _goals_handler->setEnabled(false);
        _goals_handler->resetCurrentGoal();
    }
}

void SingleplayerHandler::setHeartTargetAlpha(float alpha, bool animated) {
    if (!animated) {
        _heart_alpha = alpha;
        _delegate->setButtonInfoAlpha(_heart_alpha);
    }
    _heart_target_alpha = alpha;
}

void SingleplayerHandler::showCue() {
    _cue_animation_running = true;
    resetCue();
}

void SingleplayerHandler::showUpgradeNotification() {
    _notification_animation_running = true;
    _notification_animation_progress = 0.f;
}

void SingleplayerHandler::startScoreAnimation() {
    _score_animation_running = true;
    _last_score = _state.score;
}

void SingleplayerHandler::startGame() {
    if (_state.game_type == game_type::highscore) {
        _supercharger_target_alpha = superchargerAlpha();
    } else {
        _goals_handler->setCurrentGoal(_state.goal);
        _goals_handler->beginShot();
    }
}

void SingleplayerHandler::endGame() {
    _info_display_time_left = 0.f;
    if (_state.game_type == game_type::highscore) {
        _delegate->showGameOverView(_state.score, _state.score > _state.highscore);
        updateHighscore();
    } else {
        resetGame();
    }
    _delegate->setUpgradeNotificationAlpha(0.f);
}

void SingleplayerHandler::resetGame() {
    if (_score_animation_running) {
        _score_animation_running = false;
        if (_state.score) {
            _delegate->addChipProgress(_state.score);
        }
    }
    _base_state->waiting_for_user = false;
    _missed = false;
    _state.continued_game_count = 0;
    _state.supercharge_count >>= 2;
    _state.supercharge_count <<= 2;
    _state.score = 0;
    _last_score = 0;
    dischargeChips();
    _scene_delegate->restoreChips({});
    
    achievements_handler->submit();
    _goals_handler->submit();
    _delegate->setScore(0);
    save();
}

void SingleplayerHandler::setIntroSeen() {
    if (_state.charging_intro_seen) {
        _state.swap_intro_seen = true;
    } else {
        _state.charging_intro_seen = true;
    }
    _charging_enabled = true;
    save();
}

void SingleplayerHandler::skillShotAchieved(skill_shot_type type) {
    if (type < skill_shot_type_range) {
        _scene_delegate->skillShotAchieved(type);
        addScore(_skill_shot_handler.pointsForSkillShot(type));
        achievements_handler->skillShotAchieved(type);
        _goals_handler->skillShotAchieved(type);
        setHeartTargetAlpha(0.f);
    }
}

void SingleplayerHandler::setNotification(skill_shot_type type) {
    _delegate->setNotification(type);
}

void SingleplayerHandler::goalCompleted(unsigned short id) {
    _base_state->waiting_for_user = true;
    _state.progress = 0;
    _state.goal++;
    if (_state.goal >= goals_count) {
        _base_state->max_supercharge_count = 1;
    }
    save();
    if (_state.goal == goals_for_hearts) {
        setHeartTargetAlpha(0.f);
    }
    _delegate->showGoalCompletedView();
}

void SingleplayerHandler::setProgress(unsigned long long progress) {
    _state.progress = progress;
    _delegate->setProgress(_goals_handler->progress(), _goals_handler->count());
}

void SingleplayerHandler::setVideoType(video_type type) {
    _delegate->setVideoType(type);
}

void SingleplayerHandler::supercharge() {
    const auto per_game_count = (_state.supercharge_count & 3) + 1;
    const auto overall_count = (_state.supercharge_count >> 2) + 1;
    _state.supercharge_count = (overall_count << 2) + per_game_count;
    _supercharger_target_alpha = ui_disabled_alpha;
    _ability_used = false;
    _base_state->cue_visible = false;
    chargeChips(1.f);
    checkExtraChipNeeded();
    _skill_shot_handler.setEnabled(false);
    achievements_handler->supercharged();
}

bool SingleplayerHandler::gameStarted() const {
    return _state.score > 0 || IGameHandler::hit();
}

unsigned long long SingleplayerHandler::lastScore() const {
    return _last_score;
}

void SingleplayerHandler::updateHighscore(bool post_to_leaderboard) {
    if (_state.score > _state.highscore) {
        _state.highscore = _state.score;
        _delegate->setHighscore(_state.highscore, post_to_leaderboard);
    }
}

void SingleplayerHandler::updateScore(unsigned short &score) {
    if (_base_state->chip_type == chip_type_extra_points && chipsCharged()) {
        _base_state->chips_data[0].level > 2 ? score *= 2 : ++score;
    }
}

void SingleplayerHandler::addScore(unsigned short score) {
    if (score == 0)
        return;

    chargeChips(score / _score_charge_reduction_coeff);
    _state.score += score;
    _shot_score += score;
    if (_state.game_type == game_type::highscore) {
        _delegate->setShotScore(_shot_score);
    }
    _goals_handler->addScore(score);
}

void SingleplayerHandler::resetCue() {
    if (_base_state->waiting_for_user || _base_state->chips_data.size() > default_chips_count)
        return;

    switch ((!gameStarted() << 1) |
            (_current_chip_shot_attempt >= _base_state->chips_data.size() &&
             _widest_valid_shot_angle_found > _min_consecutive_angle_iterations)) {
        case 0b10:
        case 0b11:
            _indicator_cue = _cue_animation_state.cue;
        case 0b01:
            _base_state->cue.p = _indicator_cue.p;
            _base_state->cue_visible = true;
            _base_state->cue_animation_time_s = 0.f;
            _cue_animation_running = true;
    }
}

void SingleplayerHandler::checkExtraChipNeeded() {
    if (_base_state->chip_type == chip_type_extra_chip && chipsCharged()) {
        _goals_handler->chipsCharged();
        _base_state->ability_time_left = 1.f;
        _scene_delegate->setChargedAnimation(true, true);
        _scene_delegate->addExtraChips(_base_state->chips_data[0].level > 2 ? 2 : 1);
        _ability_used = true;
    }
}

void SingleplayerHandler::chargeChips(float charge) {
    const auto type = _base_state->chip_type;
    if (type == chip_type_default || !_charging_enabled)
        return;

    if (_base_state->charge_level + 1e-5f > 1.f) {
        if (charge < 0.f) {
            _base_state->ability_time_left += charge;
            if (_base_state->ability_time_left < 1e-5f) {
                _base_state->charge_level = 0.f;
                _base_state->ability_time_left = 0.f;
                _supercharger_target_alpha = superchargerAlpha();
                if (type == chip_type_fast) {
                    if (IGameHandler::hit()) {
                        _ability_used = true;
                    }
                    _scene_delegate->setChipsSpeedCoefficient(1.f);
                }
                _scene_delegate->setChargedAnimation(false);
            }
            _charge_progress = _base_state->ability_time_left / defaultAbilityTime();
        }
    } else {
        if (charge < 0.f) {
            if (_base_state->charge_level > 1e-5f) {
                _base_state->charge_level += charge * _discharge_coeff;
                if (_base_state->charge_level < 1e-5f) {
                    _base_state->charge_level = 0.f;
                }
            }
            if (_base_state->charge_level > _charge_progress) {
                _charge_progress -= charge;
                if (_charge_progress > _base_state->charge_level) {
                    _charge_progress = _base_state->charge_level;
                }
            } else {
                _charge_progress += charge;
                if (_charge_progress < _base_state->charge_level) {
                    _charge_progress = _base_state->charge_level;
                }
            }
        } else if (charge > 1e-5f) {
            if (_base_state->chips_data.size() > default_chips_count || _ability_used)
                return;

            if (_base_state->chips_data[0].level > 1) {
                charge *= _faster_charging_coeff;
            }
            _base_state->charge_level += charge;

            if (_base_state->charge_level + 1e-5f > 1.f) {
                _base_state->charge_level = 1.f;
                if (type == chip_type_fast) {
                    if (_base_state->chips_data[0].level > 2) {
                        _scene_delegate->setChipsSpeedCoefficient(_fast_level_2_speed_coeff);
                    } else {
                        _scene_delegate->setChipsSpeedCoefficient(_fast_level_1_speed_coeff);
                    }
                }
                _base_state->ability_time_left = defaultAbilityTime();
                if (type != chip_type_extra_chip) {
                    _goals_handler->chipsCharged();
                    _scene_delegate->setChargedAnimation(true, true);
                    _supercharger_target_alpha = superchargerAlpha();
                    if (!_state.swap_intro_seen && type == chip_type_swap) {
                        _charging_enabled = false;
                        _delegate->showIntro(intro_type_swap);
                    }
                }
            }
        }
    }
}

void SingleplayerHandler::dischargeChips() {
    _base_state->charge_level = 0.f;
    _base_state->ability_time_left = 0.f;
    _scene_delegate->setChargedAnimation(false);
    _scene_delegate->setChipsSpeedCoefficient(1.f);
}

bool SingleplayerHandler::chipsCharged() const {
    return _base_state->ability_time_left > 1e-5f;
}

float SingleplayerHandler::superchargerAlpha() const {
    if (_state.goal >= goals_count) {
        if (_base_state->chip_type != chip_type_default &&
            _base_state->chips_data.size() == default_chips_count &&
            _base_state->max_supercharge_count > (_state.supercharge_count & 3) &&
            !chipsCharged() && !_hit) {
            return 1.f;
        } else {
            return ui_disabled_alpha;
        }
    } else {
        return 0.f;
    }
}

float SingleplayerHandler::defaultAbilityTime() const {
    if (_base_state->chip_type != chip_type_swap)
        return _default_level_1_ability_time;

    return _base_state->chips_data[0].level > 2 ? _default_level_2_ability_time : _default_level_1_ability_time;
}

void SingleplayerHandler::continueHelperShotComputation() {
    for (; _current_chip_shot_attempt < _base_state->chips_data.size(); ++_current_chip_shot_attempt) {
        if (_base_state->chips_data[_current_chip_shot_attempt].icon != glyphs::blank_chip) {
            int j;
            for (j = 0; j + _current_angle_attempt < _max_360_degree_iterations && j < 10; ++j) {
                base_state chips = *_base_state;
                double try_this_angle = _subangle * (j + _current_angle_attempt);
                double x = cos(try_this_angle * M_PI / 180.0);
                double y = sin(try_this_angle * M_PI / 180.0);
                chips.chips_data[_current_chip_shot_attempt].v = {x * 40.0, y * 40.0};
                chips.last_index = _current_chip_shot_attempt;
                _valid_shot_angle[j + _current_angle_attempt] = !cuePathCollidesWithChips(x, y) &&
                        !cueOutsideBounds(x, y, _base_state->bounds) && _scene_delegate->willPass(chips);

                if (_valid_shot_angle[j + _current_angle_attempt]) {
                    if (_first_pass) {
                        ++_start_consecutive_successful_attempts_cnt;
                    } else {
                        ++_end_consecutive_successful_attempts_cnt;
                    }
                } else {
                    if (_end_consecutive_successful_attempts_cnt > _min_consecutive_angle_iterations &&
                        _end_consecutive_successful_attempts_cnt > _widest_valid_shot_angle_found) {
                        _widest_valid_shot_angle_found = _end_consecutive_successful_attempts_cnt;
                        _end_consecutive_successful_attempts_cnt /= 2;
                        setIndicatorCue(j + _current_angle_attempt - _end_consecutive_successful_attempts_cnt);
                    }
                    _first_pass = false;
                    _end_consecutive_successful_attempts_cnt = 0;
                }
            }
            _current_angle_attempt += j;
            if (_current_angle_attempt >= _max_360_degree_iterations) {
                checkIndicatorCueAround0Degrees();
                _start_consecutive_successful_attempts_cnt = 0;
                _end_consecutive_successful_attempts_cnt = 0;
                _first_pass = true;
                _current_angle_attempt = 0;
                _current_chip_shot_attempt++;
            }
            break;
        }
    }
    if (_current_chip_shot_attempt >= _base_state->chips_data.size() &&
        _widest_valid_shot_angle_found > _min_consecutive_angle_iterations) {
        resetCue();
    }
}

bool SingleplayerHandler::cuePathCollidesWithChips(double x, double y) const {
    const vec2<double> hit(x, y);
    const vec2<double> hit90(-y, x);
    const vec2<double> shot_area[4] {
        _base_state->chips_data[_current_chip_shot_attempt].p + hit90 * (Chip::default_size * 2.0),
        shot_area[0] - hit * (Chip::default_size * 6),
        shot_area[1] - hit90 * (Chip::default_size * 4),
        shot_area[0] - hit90 * (Chip::default_size * 4)
    };
    for (int k = 0; k < _base_state->chips_data.size(); ++k) {
        if (k != _current_chip_shot_attempt) {
            bool cue_path_works_for_chip = false;
            for (int v = 0; v < 4; ++v) {
                const auto rect_edge = shot_area[(v+1)%4] - shot_area[v];
                const auto vertex_chip_vec = _base_state->chips_data[k].p - shot_area[v];
                if (rect_edge.cross(vertex_chip_vec) < -1e-6) {
                    cue_path_works_for_chip = true;
                    break;
                }
            }
            if (!cue_path_works_for_chip) {
                return true;
            }
        }
    }
    return false;
}

bool SingleplayerHandler::cueOutsideBounds(double x, double y, const rect<double> &bounds) const {
    const vec2<double> cue_position = {
        _base_state->chips_data[_current_chip_shot_attempt].p.x - x * 4.0 * Chip::default_size,
        _base_state->chips_data[_current_chip_shot_attempt].p.y - y * 4.0 * Chip::default_size
    };
    return bounds.origin.x + Chip::default_size > cue_position.x ||
           cue_position.x + Chip::default_size > bounds.origin.x + bounds.size.x ||
           bounds.origin.y + Chip::default_size > cue_position.y ||
           cue_position.y + Chip::default_size > bounds.origin.y + bounds.size.y;
}

void SingleplayerHandler::checkIndicatorCueAround0Degrees() {
    unsigned short iterations = _end_consecutive_successful_attempts_cnt + _start_consecutive_successful_attempts_cnt;
    if (iterations > _min_consecutive_angle_iterations && iterations > _widest_valid_shot_angle_found) {
        _widest_valid_shot_angle_found = iterations;
        const short p = (_start_consecutive_successful_attempts_cnt - _end_consecutive_successful_attempts_cnt) / 2;
        setIndicatorCue(p < 0 ? p + _max_360_degree_iterations : p);
    }
}

void SingleplayerHandler::setIndicatorCue(short position) {
    const double a = _subangle * static_cast<double>(position) * M_PI / 180.0;
    _indicator_cue.v = {cos(a) * 4.0 * Chip::default_size, sin(a) * 4.0 * Chip::default_size};
    _indicator_cue.p = _base_state->chips_data[_current_chip_shot_attempt].p - _indicator_cue.v;
}
