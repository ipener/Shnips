// Copyright © 2018 Igor Pener. All rights reserved.

#include "TutorialHandler.hpp"

#include <algorithm>
#include "assets.hpp"
#include "ITutorialHandlerDelegate.hpp"
#include "ITutorialSceneDelegate.hpp"
#include "Logger.hpp"

using namespace std;
using namespace assets;

TutorialHandler::TutorialHandler(::base_state *base_state, IStateHandler *state_handler) : GameStateHandler<app_state::tutorial>(base_state, state_handler) {
}

void TutorialHandler::init(ITutorialHandlerDelegate *delegate, ITutorialSceneDelegate *scene_delegate) {
    _delegate = delegate;
    _scene_delegate = scene_delegate;

    if (!load()) {
        logi << "TutorialHandler failed reading state" << endl;
    }
    setShotStates();
}

void TutorialHandler::passed(unsigned short, unsigned short) {
    if (_base_state->shot_data.max_cushions_before_pass < _base_state->shot_data.cushions_after_last_pass) {
        _base_state->shot_data.max_cushions_before_pass = _base_state->shot_data.cushions_after_last_pass;
    }
    _shot_score += 1 + _base_state->shot_data.cushions_after_last_pass;
    _delegate->setTutorialShotScore(_shot_score);

    _base_state->shot_data.cushions_after_last_pass = 0;
    _base_state->shot_data.passes++;
}

void TutorialHandler::hit(unsigned short i) {
    if (i < 0xFFFF) {
        _first_shot_speed_adjusted = false;
        if (_current_tutorial == tutorial_type_simple_shot && !_state.completed) {
            const auto bottom_y = _shot_states[tutorial_type_simple_shot].chip_positions[2].y;
            if (fabs(_base_state->chips_data[i].p.y - bottom_y) < 1e-6) {
                const auto a = _base_state->chips_data[(i + 2) % default_chips_count].p - _base_state->chips_data[i].p;
                const auto b = _base_state->chips_data[(i + 1) % default_chips_count].p - _base_state->chips_data[i].p;
                if (b.cross(_base_state->cue.v) * a.cross(_base_state->cue.v) < 0.0 &&
                    _base_state->cue.v.x > 1.0 && _base_state->cue.v.y > 1.0) {
                    if (_base_state->cue.v.norm() + 1e-5 > _first_shot_pass_velocity.norm()) {
                        _first_shot_speed_adjusted = true;
                        _base_state->chips_data[i].v = _first_shot_velocity;
                    }
                }
            }
        }
        _cue_animation_running = _touching_field = false;
        IGameHandler::hit(i);
    }
}

void TutorialHandler::cushionHit() {
    _base_state->shot_data.cushions_after_last_pass++;
}

void TutorialHandler::submit() {
    _score += _shot_score;
    _shot_succeeded = false;

    const bool simple_shot_failed = _base_state->shot_data.passes == 0;
    const bool cushion_shot_failed = simple_shot_failed || _base_state->shot_data.max_cushions_before_pass == 0;
    if (_current_tutorial == tutorial_type_simple_shot && simple_shot_failed) {
        _score = 0;
        _scene_delegate->restoreChips(_shot_states[_current_tutorial].chip_positions);
    } else if (_current_tutorial == tutorial_type_consecutive_shot && simple_shot_failed) {
        _score = 0;
        _consecutive_shot_failed = true;
        _current_tutorial = tutorial_type_simple_shot;
        _notification_animation_running = true;
        _notification_animation_progress = 0.f;
        _scene_delegate->restoreChips(_shot_states[_current_tutorial].chip_positions);
    } else if (_current_tutorial == tutorial_type_cushion_shot && cushion_shot_failed) {
        _score = 0;
        _scene_delegate->restoreChips(_shot_states[_current_tutorial].chip_positions);
    } else if (_current_tutorial == tutorial_type_corner_shot && cushion_shot_failed) {
        _score = 0;
        _scene_delegate->restoreChips(_shot_states[_current_tutorial].chip_positions);
    } else {
        _shot_succeeded = true;
        if (_current_tutorial == tutorial_type_simple_shot && _consecutive_shot_failed) {
            _notification_animation_progress = 0.f;
            _notification_animation_running = true;
            _current_tutorial = static_cast<tutorial_type>(_current_tutorial + 1);
            maybeShowCue();
        } else {
            _delegate->showAlert(_current_tutorial);
            _current_tutorial = static_cast<tutorial_type>(_current_tutorial + 1);
        }
    }

    _delegate->setTutorialScore(_score);

    _base_state->shot_data = {};
    _shot_score = 0;
    IGameHandler::submit();
}

void TutorialHandler::update(float t) {
    if (_notification_animation_running) {
        const auto progress = _notification_animation_progress;
        _notification_animation_progress += t * ui_transition_step;
        if (_notification_animation_progress > 0.99999f) {
            _notification_animation_progress = 1.f;
            _notification_animation_running = false;
        }
        if (_consecutive_shot_failed &&
            (_current_tutorial == tutorial_type_simple_shot || _current_tutorial == tutorial_type_consecutive_shot)) {
            if (_notification_animation_progress > 0.5f && progress <= 0.5f) {
                _delegate->setTutorialTitle(_current_tutorial);
            }
            if (_notification_animation_progress < 0.5f) {
                _delegate->setTutorialTitleAlpha(1.f - 2.f * _notification_animation_progress);
            } else {
                _delegate->setTutorialTitleAlpha(2.f * _notification_animation_progress - 1.f);
            }
        } else {
            const auto m = 1.f + ui_background_view_alpha;
            const auto x = 2.f * ui_background_view_alpha / 2.f / m;
            if (_notification_animation_progress > x && progress <= x) {
                _delegate->setTutorialTitle(_current_tutorial);
            }
            if (_notification_animation_progress < x) {
                _delegate->setTutorialTitleAlpha(ui_background_view_alpha - m * _notification_animation_progress);
            } else {
                _delegate->setTutorialTitleAlpha(m * _notification_animation_progress - ui_background_view_alpha);
            }
        }
    }

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
}

void TutorialHandler::touchesBegan(const vec2<double> &p, long long t_ms) {
    if (_notification_animation_running)
        return;

    _touching_field = _base_state->cue_visible = true;
    _base_state->cue_animation_time_s = touch_hide_time_s;
}

void TutorialHandler::touchesEnded(const vec2<double> &p, long long t_ms) {
    if (_notification_animation_running)
        return;

    _touching_field = _base_state->cue_visible = false;
    if (_cue_animation_running && shouldShowCue()) {
        resetCue();
    }
}

void TutorialHandler::onChipsRestored() {
    const auto type = _base_state->chips_data[0].type;
    if (_current_tutorial == tutorial_type_cushion_shot) {
        sort(_base_state->chips_data.begin(), _base_state->chips_data.end(), [](const Chip &c0, const Chip &c1) -> bool {
            return c0.p.y < c1.p.y;
        });
        _base_state->last_index = 1;
        _base_state->chips_data[0].icon = glyphs::chips[type];
        _base_state->chips_data[2].icon = glyphs::chips[type];
        _base_state->chips_data[1].icon = glyphs::blank_chip;
    } else if (_current_tutorial == tutorial_type_corner_shot) {
        sort(_base_state->chips_data.begin(), _base_state->chips_data.end(), [](const Chip &c0, const Chip &c1) -> bool {
            return c0.p.x < c1.p.x;
        });
        _base_state->last_index = 1;
        _base_state->chips_data[0].icon = glyphs::chips[type];
        _base_state->chips_data[2].icon = glyphs::chips[type];
        _base_state->chips_data[1].icon = glyphs::blank_chip;
    } else {
        if (_base_state->last_index < default_chips_count) {
            _base_state->chips_data[_base_state->last_index].icon = glyphs::chips[type];
        }
        _base_state->last_index = 0xFFFF;
        if (_current_tutorial == tutorial_type_range) {
            _current_tutorial = tutorial_type_simple_shot;
            return _delegate->exitTutorial();
        }
    }
    maybeShowCue();
}

void TutorialHandler::willBecomeActive() {
    _delegate->setCancelable(_state.completed);
}

bool TutorialHandler::collisionEnabled() const {
    return _touching_field || _hit;
}

bool TutorialHandler::playableGame() const {
    return true;
}

void TutorialHandler::showNextTutorial() {
    if (_current_tutorial == tutorial_type_range) {
        if (!_state.completed) {
            _state.completed = true;
            save();
        }
        _consecutive_shot_failed = false;
        _score = 0;
        _scene_delegate->restoreChips(_shot_states[tutorial_type_simple_shot].chip_positions);
    } else {
        if (_current_tutorial == tutorial_type_simple_shot) {
            _delegate->setTutorialScore(_score);
            _delegate->setTutorialTitle(_current_tutorial);
            _delegate->setTutorialTitleAlpha(1.f);
        } else {
            _notification_animation_running = true;
            _notification_animation_progress = 0.f;
        }
        if (_current_tutorial != tutorial_type_consecutive_shot) {
            _scene_delegate->restoreChips(_shot_states[_current_tutorial].chip_positions);
        }
        _shot_succeeded = _touching_field = false;
        if (shouldShowCue()) {
            _cue_animation_running = true;
        }
    }
}

void TutorialHandler::cancelTutorial() {
    _base_state->cue_visible = false;
    _current_tutorial = tutorial_type_range;
    _consecutive_shot_failed = false;
    _score = 0;
    _scene_delegate->restoreChips(_shot_states[tutorial_type_simple_shot].chip_positions);
}

// MARK: - private

void TutorialHandler::resetCue() {
    if (_current_tutorial < tutorial_type_range) {
        _indicator_cue = _shot_states[_current_tutorial].cue;
        _base_state->cue.p = _indicator_cue.p;
    }
    if (_cue_animation_running) {
        _base_state->cue_visible = true;
    }
    _base_state->cue_animation_time_s = 0.f;
}

void TutorialHandler::setShotStates() {
    Chip::defaultShotState(_shot_states[tutorial_type_simple_shot]);

    // These chip positions should be fine based on: https://design.google.com/devices/
    _shot_states[tutorial_type_cushion_shot].chip_positions = {
        {screen_size.x / 2.0, screen_size.y * 0.3333 - 3.0 * Chip::default_size},
        {screen_size.x / 2.0, screen_size.y * 0.3333},
        {screen_size.x * 0.75, screen_size.y * 0.6667}
    };
    auto p = _shot_states[tutorial_type_cushion_shot].chip_positions[0];
    _shot_states[tutorial_type_cushion_shot].cue.r = Chip::default_size;
    _shot_states[tutorial_type_cushion_shot].cue.p = {p.x / 3.0, p.y - 2.0 * p.x / (3.0 * sqrt(3.0))};
    auto d = p - _shot_states[tutorial_type_cushion_shot].cue.p;
    _shot_states[tutorial_type_cushion_shot].cue.v.y = sqrt(d.x * d.x + d.y * d.y) * 0.5;
    _shot_states[tutorial_type_cushion_shot].cue.v.x = _shot_states[tutorial_type_cushion_shot].cue.v.y * sqrt(3.0);

    // Simple shot
    const auto v_pass = (_shot_states[tutorial_type_simple_shot].chip_positions[0] - _shot_states[tutorial_type_simple_shot].chip_positions[2]) + (_shot_states[tutorial_type_simple_shot].chip_positions[1] - _shot_states[tutorial_type_simple_shot].chip_positions[0]) * 0.5;
    const auto p_pass = _shot_states[tutorial_type_simple_shot].chip_positions[2] + v_pass;
    const auto p_pass_long = p_pass + v_pass;
    auto v = _shot_states[tutorial_type_simple_shot].cue.v;
    v.normalize();
    const auto bottom_y = _shot_states[tutorial_type_simple_shot].chip_positions[2].y;
    const auto l_pass = sqrt((p_pass.y - bottom_y) * (2.0 * assets::deceleration_factor) / v.y);
    const auto len = sqrt((p_pass_long.y - bottom_y) * (2.0 * assets::deceleration_factor) / v.y);
    _first_shot_pass_velocity = v * l_pass;
    _first_shot_velocity = v * len;

    // Consecutive shot
    _shot_states[tutorial_type_consecutive_shot].cue.r = Chip::default_size;
    _shot_states[tutorial_type_consecutive_shot].cue.p = _shot_states[tutorial_type_simple_shot].chip_positions[1];
    _shot_states[tutorial_type_consecutive_shot].cue.p.x -= Chip::default_size * 4.0;
    if (_shot_states[tutorial_type_consecutive_shot].cue.p.x < Chip::default_size) {
        _shot_states[tutorial_type_consecutive_shot].cue.p.x = Chip::default_size;
    }
    _shot_states[tutorial_type_consecutive_shot].cue.v = {
        _shot_states[tutorial_type_simple_shot].chip_positions[1].x - _shot_states[tutorial_type_consecutive_shot].cue.p.x,
        0.0
    };

    // Corner shot
    _shot_states[tutorial_type_corner_shot].chip_positions = {
        {1.5 * Chip::default_size, 1.5 * Chip::default_size},
        {screen_size.x / 2.0, screen_size.y * 0.6667},
        {screen_size.x - 1.5 * Chip::default_size, 1.5 * Chip::default_size},
    };
    _shot_states[tutorial_type_corner_shot].cue.r = Chip::default_size;
    _shot_states[tutorial_type_corner_shot].cue.p = {screen_size.x / 2.0, screen_size.x / 2.0};
    d = _shot_states[tutorial_type_corner_shot].chip_positions[0] - _shot_states[tutorial_type_corner_shot].cue.p;
    _shot_states[tutorial_type_corner_shot].cue.v.y = -sqrt((d.x * d.x + d.y * d.y) / 2.0);
    _shot_states[tutorial_type_corner_shot].cue.v.x = _shot_states[tutorial_type_corner_shot].cue.v.y;
}

void TutorialHandler::maybeShowCue() {
    if (shouldShowCue()) {
        _cue_animation_running = true;
        _base_state->cue_visible = true;
        resetCue();
    }
}

bool TutorialHandler::shouldShowCue() const {
    return (
        _current_tutorial == tutorial_type_simple_shot ||
        _current_tutorial == tutorial_type_cushion_shot ||
        _current_tutorial == tutorial_type_corner_shot ||
        (_current_tutorial == tutorial_type_consecutive_shot && !_state.completed && _first_shot_speed_adjusted)
    );
}
