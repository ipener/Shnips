// Copyright © 2018 Igor Pener. All rights reserved.

#include "VideoHandler.hpp"

#include "assets.hpp"
#include "IVideoSceneDelegate.hpp"

using namespace assets;

VideoHandler::VideoHandler(::base_state *base_state) : IGameHandler(base_state), _slide_x_offset((screen_size.x + video_view_size) / 2.f) {
    _view_bounds = {
        {0.f, 2.f * ui_paragraph_margin * video_view_size / screen_size.x},
        {video_view_size, video_view_size}
    };
}

void VideoHandler::init(IVideoSceneDelegate *scene_delegate) {
    _scene_delegate = scene_delegate;
    const auto p = _view_bounds.origin;

    _video_states[video_type_indirect_pass] = {
        {{100.0 + p.x, 300.0 + p.y}, {250.0 + p.x, 300.0 + p.y}, {200.0 + p.x, 200.0 + p.y}},
        {{150.0 + p.x, 100.0 + p.y}, {50.0, 100.0}, Chip::default_size},
        0.18f
    };
    _video_states[video_type_just_passed] = {
        {{100.0 + p.x, 300.0 + p.y}, {300.0 + p.x, 300.0 + p.y}, {150.0 + p.x, 200.0 + p.y}},
        {{100.0 + p.x, 100.0 + p.y}, {50.0, 100.0}, Chip::default_size},
        0.075f
    };
    _video_states[video_type_narrow_gap] = {
        {{200.0 + p.x, 300.0 + p.y}, {300.0 + p.x, 200.0 + p.y}, {150.0 + p.x, 150.0 + p.y}},
        {{50.0 + p.x, 50.0 + p.y}, {100.0, 100.0}, Chip::default_size},
        0.09f
    };
    _video_states[video_type_three_cushion_hit] = {
        {{50.0 + p.x, 350.0 + p.y}, {200.0 + p.x, 250.0 + p.y}, {200.0 + p.x, 50.0 + p.y}},
        {{100.0 + p.x, 100.0 + p.y}, {100.0, -50.0}, Chip::default_size},
        0.3f
    };
    _video_states[video_type_four_passes] = {
        {{100.0 + p.x, 350.0 + p.y}, {300.0 + p.x, 350.0 + p.y}, {200.0 + p.x, 250.0 + p.y}},
        {{200.0 + p.x, 50.0 + p.y}, {0.0, 200.0}, Chip::default_size},
        0.175f
    };
    _video_states[video_type_chips_close] = {
        {{165.0 + p.x, 300.0 + p.y}, {235.0 + p.x, 300.0 + p.y}, {200.0 + p.x, 250.0 + p.y}},
        {{200.0 + p.x, 50.0 + p.y}, {0.0, 200.0}, Chip::default_size},
        0.15f
    };
    _video_states[video_type_two_passes_no_cushion] = {
        {{300.0 + p.x, 50.0 + p.y}, {350.0 + p.x, 150.0 + p.y}, {200.0 + p.x, 100.0 + p.y}},
        {{50.0 + p.x, 50.0 + p.y}, {150.0, 50.0}, Chip::default_size},
        0.22f
    };
    _video_states[video_type_circumvent_chip] = {
        {{350.0 + p.x, 50.0 + p.y}, {150.0 + p.x, 200.0 + p.y}, {350.0 + p.x, 150.0 + p.y}},
        {{250.0 + p.x, 50.0 + p.y}, {100.0, 100.0}, Chip::default_size},
        0.25f
    };
    _video_states[video_type_hit_chips_before_pass] = {
        {{250.0 + p.x, 260.0 + p.y}, {100.0 + p.x, 250.0 + p.y}, {100.0 + p.x, 150.0 + p.y}},
        {{50.0 + p.x, 50.0 + p.y}, {50.0, 100.0}, Chip::default_size},
        0.12f
    };
    _video_states[video_type_no_cushion_shot] = {
        {{150.0 + p.x, 300.0 + p.y}, {300.0 + p.x, 150.0 + p.y}, {150.0 + p.x, 150.0 + p.y}},
        {{50.0 + p.x, 50.0 + p.y}, {100.0, 100.0}, Chip::default_size},
        0.08f
    };
    _video_states[video_type_double_cushion_shot] = {
        {{50.0 + p.x, 300.0 + p.y}, {200.0 + p.x, 200.0 + p.y}, {300.0 + p.x, 200.0 + p.y}},
        {{200.0 + p.x, 100.0 + p.y}, {100.0, 100.0}, Chip::default_size},
        0.21f
    };
    _video_states[video_type_double_pass] = {
        {{250.0 + p.x, 300.0 + p.y}, {300.0 + p.x, 100.0 + p.y}, {200.0 + p.x, 200.0 + p.y}},
        {{100.0 + p.x, 200.0 + p.y}, {100.0, 0.0}, Chip::default_size},
        0.22f
    };
    _video_states[video_type_triple_pass] = {
        {{200.0 + p.x, 300.0 + p.y}, {250.0 + p.x, 100.0 + p.y}, {150.0 + p.x, 200.0 + p.y}},
        {{50.0 + p.x, 200.0 + p.y}, {100.0, 0.0}, Chip::default_size},
        0.35f
    };
}

void VideoHandler::hit(unsigned short i) {
    IGameHandler::hit(i);
    _base_state->cue_visible = false;
}

void VideoHandler::update(float t) {
    if (_video_playing) {
        _base_state->cue_animation_time_s += t;

        if (_base_state->cue_animation_time_s + 1e-5f > _video_duration) {
            _hit = false;
            _indicator_cue = _video_states[_current_video].cue;
            _base_state->cue = _indicator_cue;
            _base_state->cue.v *= _video_states[_current_video].speed_coefficient;
            _scene_delegate->restoreChips(_video_states[_current_video].chip_positions, _video_last_index, false);
        } else if (_base_state->cue_animation_time_s > touch_hide_time_s &&
                   _base_state->cue_animation_time_s < cue_hide_time_s && !_hit) {
            _base_state->cue.p = _indicator_cue.p + _indicator_cue.v * (_base_state->cue_animation_time_s - touch_hide_time_s) / (cue_hide_time_s - touch_hide_time_s);
            _scene_delegate->collideWithCue();
        } else if (_base_state->cue_animation_time_s > cue_hide_time_s) {
            _base_state->cue_visible = false;
        }
    }
}

void VideoHandler::onChipsRestored() {
    _base_state->cue_animation_time_s = 0.f;
    _base_state->cue_visible = true;
}

void VideoHandler::willBecomeActive() {
    _initial_chips_data = _base_state->chips_data;
    _initial_last_index = _base_state->last_index;
    _initial_bounds = _base_state->bounds;

    _hit = false;
    _indicator_cue = _video_states[_current_video].cue;
    _base_state->cue_animation_time_s = 0.f;
    _base_state->cue = _indicator_cue;
    _base_state->cue.v *= _video_states[_current_video].speed_coefficient;
    _base_state->last_index = _video_last_index;
    _base_state->chips_data.clear();
    _base_state->cue_visible = false;

    const auto size = _video_states[_current_video].chip_positions.size();
    const auto type = _initial_chips_data[0].type;
    const auto level = _initial_chips_data[0].level;
    for (size_t i = 0; i < size; ++i) {
        _base_state->chips_data.push_back({_video_states[_current_video].chip_positions[i], {}, type, level});
        _base_state->chips_data[i].p.x += _slide_x_offset;
        _base_state->chips_data[i].gradient = glyphs::gradients[type];
        if (i == _base_state->last_index || i >= default_chips_count) {
            _base_state->chips_data[i].icon = glyphs::blank_chip;
        } else {
            _base_state->chips_data[i].icon = glyphs::chips[type];
        }
    }
    for (const auto &chip : _initial_chips_data) {
        _base_state->chips_data.push_back(chip);
        _base_state->chips_data.back().v = {};
    }
}

void VideoHandler::willResignActive() {
    _base_state->chips_data = _initial_chips_data;
    _base_state->last_index = _initial_last_index;
    _base_state->bounds = _initial_bounds;
    _exit_transition_running = false;
}

bool VideoHandler::collisionEnabled() const {
    return _video_playing;
}

bool VideoHandler::interactionAllowed() const {
    return false;
}

void VideoHandler::setVideoType(video_type type) {
    _current_video = type;
}

void VideoHandler::setTransitionProgress(float progress) {
    if (progress < 0.f) {
        return;
    } else if (progress > 0.99999f) {
        _video_playing = true;
        _base_state->cue_visible = true;
        _base_state->bounds = _view_bounds;
        const auto size = _video_states[_current_video].chip_positions.size();
        for (size_t i = 0; i < size; ++i) {
            _base_state->chips_data[i].p.x = _video_states[_current_video].chip_positions[i].x;
        }
        while (_base_state->chips_data.size() > size) {
            _base_state->chips_data.pop_back();
        }
    } else {
        const auto size = _video_states[_current_video].chip_positions.size();
        if (_exit_transition_running) {
            for (size_t i = 0; i < size; ++i) {
                _base_state->chips_data[i].p.x += _slide_x_offset * seconds_for_60_fps * ui_transition_step * 2.f;
            }
        } else {
            for (size_t i = 0; i < size; ++i) {
                _base_state->chips_data[i].p.x = _video_states[_current_video].chip_positions[i].x + (1.f - progress) * _slide_x_offset;
            }
        }
        auto j = size;
        for (const auto &chip : _initial_chips_data) {
            _base_state->chips_data[j++].p.x = chip.p.x - progress * _slide_x_offset;
        }
    }
}

void VideoHandler::startExitTransition() {
    _video_playing = false;
    _exit_transition_running = true;

    for (auto &chip : _base_state->chips_data) {
        chip.v = {};
    }
    for (const auto &chip : _initial_chips_data) {
        _base_state->chips_data.push_back(chip);
        _base_state->chips_data.back().p.x -= _slide_x_offset;
        _base_state->chips_data.back().v = {};
    }
    _base_state->cue_visible = false;
}
