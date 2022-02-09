// Copyright © 2018 Igor Pener. All rights reserved.

#include "StoreHandler.hpp"

#include <algorithm>
#include <cmath>

#include "IStoreHandlerDelegate.hpp"
#include "IAchievementsHandler.hpp"
#include "Logger.hpp"
#include "utils.hpp"

using namespace std;
using namespace assets;

class SwipeHandler {
public:
    SwipeHandler(SwipeHandlerDelegate *delegate, float width) : _delegate(delegate), _width(width) {
    }

    void touchesBegan(const vec2<double> &p, long long t_ms) {
        _invalid_swipe = _transition_running;
        if (_invalid_swipe)
            return;

        _begin_touch_time = t_ms;
        _begin_touch_location = p;
        _transition_running = false;
    }

    void touchesMoved(const vec2<double> &p) {
        if (_invalid_swipe)
            return;

        _transition_progress = (p.x - _begin_touch_location.x) / _width;
        if ((_delegate->isFirstPage() && _transition_progress > 0.0) || (_delegate->isLastPage() && _transition_progress < 0.0)) {
            _transition_progress *= 0.25;
            if (_transition_progress > 0.4) {
                _transition_progress = 0.4;
            } else {
                if (_transition_progress < -0.4) {
                    _transition_progress = -0.4;
                }
            }
        }
    }

    void touchesEnded(const vec2<double> &p, long long t_ms) {
        if (_invalid_swipe)
            return;

        _transition_progress = (p.x - _begin_touch_location.x) / _width;
        if ((_delegate->isFirstPage() && _transition_progress > 0.0) || (_delegate->isLastPage() && _transition_progress < 0.0)) {
            _transition_progress *= 0.25;
            if (_transition_progress > 0.4) {
                _transition_progress = 0.4;
            } else if (_transition_progress < -0.4) {
                _transition_progress = -0.4;
            }
        }

        _transition_running = true;
        _end_touch_time = t_ms;
        const auto duration = static_cast<double>(_end_touch_time - _begin_touch_time);
        _dx = duration < 1e-5 ? 0.0 : _transition_progress / duration;

        if (_delegate->isFirstPage() && (_transition_progress > 0 || _dx > 0.001)) {
            _dx = 0.0;
        } else if (_delegate->isLastPage() && (_transition_progress < 0 || _dx < -0.001)) {
            _dx = 0.0;
        }
    }

    void update(float t) {
        if (_transition_running) {
            float speed = t * 5.f;

            if (_dx < -0.001) {
                speed *= -1.f;
            } else if (_dx <= 0.001) {
                if (_transition_progress < -0.5) {
                    speed *= -1.f;
                } else if (_transition_progress <= 0.5) {
                    if (_transition_progress < 0.0) {
                        if (_transition_progress + speed > 0.0)
                            return changePage(0);
                    } else {
                        speed *= -1.f;
                        if (_transition_progress + speed < 0.0)
                            return changePage(0);
                    }
                }
            }

            _transition_progress += speed;

            if (_transition_progress <= -1.0) {
                changePage(1);
            } else if (_transition_progress >= 1.0) {
                changePage(-1);
            }
        }
    }

    // Returns values from -1 to 1, used for gradient and page indicators
    double progress() const {
        return _transition_progress;
    }

private:
    void changePage(short page_delta) {
        _delegate->changePage(page_delta);
        _transition_running = false;
        _transition_progress = 0.0;
    }

private:
    SwipeHandlerDelegate *_delegate = nullptr;

    vec2<double>         _begin_touch_location;
    long long            _begin_touch_time;
    long long            _end_touch_time;
    double               _dx;
    double               _transition_progress = 0.f;
    float                _width;
    bool                 _transition_running = false;
    bool                 _invalid_swipe = false;
};

StoreHandler::~StoreHandler() {
    delete _swipe_handler;
}

void StoreHandler::init(IStoreHandlerDelegate *delegate, unsigned short goal) {
    _delegate = delegate;

    if (load()) {
        while (_state.progress.size() < chip_type_range) {
            _state.progress.push_back(0);
        }
    } else {
        logi << "StoreHandler failed reading state" << endl;
        _state.progress.clear();
        _state.progress.resize(chip_type_range, 0);
    }
    _page_index = _base_state->chip_type;

    _chips_progress[chip_type_default] = {{0, 100}, 0};
    _chips_progress[chip_type_fast] = {{1000, 2000}, 15};
    _chips_progress[chip_type_extra_points] = {{2000, 4000}, 30};
    _chips_progress[chip_type_swap] = {{3000, 6000}, 45};
    _chips_progress[chip_type_extra_chip] = {{4000, 8000}, 60};

    _prices[in_app_purchase_type_hearts] = L"—";

    setGoal(goal);
    _unlocked_chip = chip_type_range;

    const auto size = _base_state->chips_data.size();
    for (size_t i = 0; i < size; ++i) {
        _base_state->chips_data[i].type = _base_state->chip_type;
        _base_state->chips_data[i].level = _current_levels[_base_state->chip_type];
        _base_state->chips_data[i].gradient = glyphs::gradients[_base_state->chip_type];
        if (i == _base_state->last_index || i >= default_chips_count) {
            _base_state->chips_data[i].icon = glyphs::blank_chip;
        } else {
            _base_state->chips_data[i].icon = glyphs::chips[_base_state->chip_type];
        }
    }

    for (unsigned short i = 0; i < chip_type_range; ++i) {
        if (_current_levels[i]) {
            _delegate->setIndicatorLevel(i, _current_levels[i]);
        }
    }

    _chips_offset = {8.f * Chip::default_size * screen_size.x / screen_size.y, 4.f * Chip::default_size};
    _swipe_handler = new SwipeHandler(this, screen_size.x);
}

void StoreHandler::update(float t) {
    _swipe_handler->update(t);

    if (_interaction_allowed) {
        const auto size = _base_state->chips_data.size();
        for (size_t i = 0; i < size; ++i) {
            _base_state->chips_data[i].p.x = _initial_chips_data[_focused_chip_index].p.x + _chips_offset.x * (_swipe_handler->progress() + i - _page_index);
        }
    }
    const float swipe_progress = static_cast<float>(_swipe_handler->progress());
    if (swipe_progress < 0.f && _page_index < chip_type_range - 1) {
        updateCurrentlyDisplayedPage(_page_index, _page_index + 1, -swipe_progress);
    } else if (swipe_progress > 0.f && _page_index > 0) {
        updateCurrentlyDisplayedPage(_page_index, _page_index - 1, swipe_progress);
    }
}

void StoreHandler::touchesBegan(const vec2<double> &p, long long t_ms) {
    if (_interaction_allowed) {
        _swipe_handler->touchesBegan(p, t_ms);
    }
}

void StoreHandler::touchesMoved(const vec2<double> &p, long long t_ms) {
    if (_interaction_allowed) {
        _swipe_handler->touchesMoved(p);
    }
}

void StoreHandler::touchesEnded(const vec2<double> &p, long long t_ms) {
    if (_interaction_allowed) {
        _swipe_handler->touchesEnded(p, t_ms);
    }
}

void StoreHandler::willBecomeActive() {
    _initial_chips_data = _base_state->chips_data;
    _initial_last_index = _base_state->last_index;
    _base_state->cue_visible = false;

    double max_h = -1e100;
    for (unsigned short i = 0; i < _base_state->chips_data.size(); ++i) {
        if (max_h < _base_state->chips_data[i].p.y) {
            max_h = _base_state->chips_data[i].p.y;
            _focused_chip_index = i;
        }
    }
    chip_type type;
    if (_unlocked_chip < chip_type_range) {
        type = _unlocked_chip;
        _unlocked_chip = chip_type_range;
        addChipForTransition(type);
    } else {
        type = _base_state->chip_type;
    }

    const auto points_required = pointsRequired(type);
    const auto p = points_required ? progress(type) / static_cast<float>(points_required) : 0.f;
    _delegate->setChipType(type, _current_levels[type], p);

    _page_index = static_cast<unsigned short>(type);
    _currently_displayed_page = _page_index;
    for (unsigned short i = 0; i < chip_type_range; ++i) {
        _delegate->setIndicatorAlpha(i, ui_disabled_alpha);
    }
    _delegate->setIndicatorAlpha(_base_state->chip_type, 1.f);
}

void StoreHandler::willResignActive() {
    _base_state->chips_data = _initial_chips_data;
    _base_state->last_index = _initial_last_index;
}

bool StoreHandler::collisionEnabled() const {
    return false;
}

void StoreHandler::setTransitionProgress(float progress) {
    if (progress < 0.f) {
        return;
    } else if (progress > 0.99999f) {
        _interaction_allowed = true;
        auto p = _initial_chips_data[_focused_chip_index].p;
        p.x -= _chips_offset.x * _page_index;
        _base_state->chips_data.resize(chip_type_range);
        for (unsigned short i = 0; i < chip_type_range; ++i, p.x += _chips_offset.x) {
            _base_state->chips_data[i].p = p;
            _base_state->chips_data[i].type = static_cast<chip_type>(i);
            _base_state->chips_data[i].level = _current_levels[i];
            _base_state->chips_data[i].icon = _current_levels[i] ? glyphs::chips[i] : glyphs::blank_chip;
            _base_state->chips_data[i].gradient = glyphs::gradients[i];
        }
    } else {
        _interaction_allowed = false;
        if (_base_state->chip_type != _page_index) {
            const float x = _chips_offset.x * progress + screen_size.x * (1.f - progress);
            const auto size = _initial_chips_data.size();
            for (size_t i = 0; i < size; ++i) {
                _base_state->chips_data[i].p.x = _initial_chips_data[i].p.x - progress * x;
                _base_state->chips_data[i].p.y = _initial_chips_data[i].p.y - progress * _chips_offset.y;
            }
            _base_state->chips_data[size].p.x = _initial_chips_data[_focused_chip_index].p.x + x - progress * x;
            updateTransitionState(_base_state->chip_type, _page_index, progress);
        }
    }
}

void StoreHandler::startExitTransition() {
    if (_current_levels[_page_index]) {
        _base_state->chip_type = static_cast<chip_type>(_page_index);
        for (auto &chip : _initial_chips_data) {
            chip.type = _base_state->chip_type;
            chip.level = _current_levels[_page_index];
            chip.icon = glyphs::chips[_page_index];
            chip.gradient = glyphs::gradients[_page_index];
        }
        _base_state->chips_data = _initial_chips_data;
        _base_state->last_index = _initial_last_index;
        save();
    } else {
        _base_state->chips_data = _initial_chips_data;
        _base_state->last_index = _initial_last_index;
        addChipForTransition(static_cast<chip_type>(_page_index));
    }
}

unsigned short StoreHandler::chipLevel() const {
    return _current_levels[_base_state->chip_type];
}

chip_type StoreHandler::unlockedChip() const {
    return _unlocked_chip;
}

chip_type StoreHandler::nextUnlockableChip() const {
    for (unsigned short i = 0; i < chip_type_range; ++i) {
        if (_current_levels[i] == 0)
            return static_cast<chip_type>(i);
    }
    return chip_type_range;
}

bool StoreHandler::isTransitionRunning() const {
    return fabs(_swipe_handler->progress()) > 1e-6;
}

bool StoreHandler::canShowGoals() const {
    return _current_levels[chip_type_default] == max_chip_level;
}

std::wstring StoreHandler::price(in_app_purchase_type type) const {
    return type < in_app_purchase_type_range ? _prices[type] : L"—";
}

unsigned short StoreHandler::goalsToUnlockNextChip() const {
    return _goals_to_unlock_next_chip;
}

unsigned short StoreHandler::currentLevel(chip_type type) const {
    return _current_levels[type];
}

unsigned long StoreHandler::pointsRequired(chip_type type) const {
    if (_current_levels[type] == 0) {
        return _chips_progress[type].points_required[0];
    } else if (_current_levels[type] == max_chip_level) {
        return _chips_progress[type].points_required[1];
    } else {
        return _chips_progress[type].points_required[_current_levels[type] - 1];
    }
}

unsigned long StoreHandler::progress(chip_type type) const {
    if (_current_levels[type] == 2) {
        return _state.progress[type] - _chips_progress[type].points_required[0];
    } else if (_current_levels[type] == max_chip_level) {
        return _chips_progress[type].points_required[1];
    } else {
        return _state.progress[type];
    }
}

void StoreHandler::goalCompleted(unsigned short goal) {
    setGoal(goal);
    save();
}

void StoreHandler::setGoal(unsigned short goal) {
    _goals_to_unlock_next_chip = assets::goals_count;
    for (short i = 0; i < chip_type_range; ++i) {
        if (goal < _chips_progress[i].goals_required) {
            _current_levels[i] = 0;
            if (_chips_progress[i].goals_required < _goals_to_unlock_next_chip) {
                _goals_to_unlock_next_chip = _chips_progress[i].goals_required;
            }
        } else {
            _current_levels[i] = 1;
            if (_chips_progress[i].goals_required == goal) {
                _unlocked_chip = static_cast<chip_type>(i);
                _delegate->setIndicatorLevel(i, _current_levels[i]);
            }
        }
        unsigned long long progress = 0;
        for (short j = 0; j < 2; ++j) {
            progress += _chips_progress[i].points_required[j];
            if (_state.progress[i] >= progress) {
                _current_levels[i]++;
            }
        }
    }
}

void StoreHandler::setPrice(in_app_purchase_type type, const std::wstring &price) {
    if (type < in_app_purchase_type_range) {
        _prices[type] = price;
    }
}

void StoreHandler::addProgress(unsigned long long progress) {
    const auto type = _base_state->chip_type;
    if (_current_levels[type] == max_chip_level || _current_levels[type] == 0)
        return;

    _state.progress[type] += progress;
    unsigned long total = _chips_progress[type].points_required[0];
    if (_current_levels[type] == max_chip_level - 1) {
        total += _chips_progress[type].points_required[1];
    }
    if (_state.progress[type] >= total) {
        _current_levels[type]++;
        _delegate->setIndicatorLevel(type, _current_levels[type]);
        for (auto &chip: _base_state->chips_data) {
            chip.level = _current_levels[type];
        }
        if (_current_levels[type] == max_chip_level) {
            _state.progress[type] = total;
        }
    }
    save();
}

// MARK: - private

void StoreHandler::updateTransitionState(unsigned short i0, unsigned short i1, float t) {
    const float s = 1.f - t;
    const float gradient[6] = {
        chip_themes[i0].gradient[0] * s + chip_themes[i1].gradient[0] * t,
        chip_themes[i0].gradient[1] * s + chip_themes[i1].gradient[1] * t,
        chip_themes[i0].gradient[2] * s + chip_themes[i1].gradient[2] * t,
        chip_themes[i0].gradient[3] * s + chip_themes[i1].gradient[3] * t,
        chip_themes[i0].gradient[4] * s + chip_themes[i1].gradient[4] * t,
        chip_themes[i0].gradient[5] * s + chip_themes[i1].gradient[5] * t
    };
    if (!_current_levels[i1]) {
        _delegate->setBackgroundData(gradient, !_current_levels[i0] ? 1.f : i0 < i1 ? t : s);
    } else if (!_current_levels[i0]) {
        _delegate->setBackgroundData(gradient, i0 < i1 ? t : s);
    } else {
        _delegate->setBackgroundData(gradient, 0.f);
    }
    if (_interaction_allowed) {
        _delegate->setIndicatorAlpha(i0, s + ui_disabled_alpha * t);
        _delegate->setIndicatorAlpha(i1, t + ui_disabled_alpha * s);
    }
}

void StoreHandler::updateCurrentlyDisplayedPage(unsigned short i0, unsigned short i1, float t) {
    if (i0 < chip_type_range && i1 < chip_type_range) {
        if (t < 0.5f && _currently_displayed_page != i0) {
            setCurrentlyDisplayedPage(i0);
        } else if (t > 0.5f && _currently_displayed_page != i1) {
            setCurrentlyDisplayedPage(i1);
        }
        updateTransitionState(i0, i1, t);
        _delegate->setViewAlpha(t < 0.5f ? 1.f - 2.f * t : 2.f * t - 1.f);
    }
}

void StoreHandler::setCurrentlyDisplayedPage(unsigned short currently_displayed_page) {
    _currently_displayed_page = currently_displayed_page;
    const auto type = static_cast<chip_type>(_currently_displayed_page);
    if (!_current_levels[type]) {
        _delegate->setChipType(type, _current_levels[type], 0.f);
        _delegate->setGoalsProgress(_chips_progress[type].goals_required);
    } else {
        const auto points_required = pointsRequired(type);
        const auto p = points_required ? progress(type) / static_cast<float>(points_required) : 0.f;
        _delegate->setChipType(type, _current_levels[type], p);
    }
}

void StoreHandler::addChipForTransition(chip_type type) {
    Chip chip(_initial_chips_data[_focused_chip_index].p + vec2<double>(screen_size.x, 0.0), {}, type, _current_levels[type]);
    chip.icon = _current_levels[type] ? glyphs::chips[type] : glyphs::blank_chip;
    chip.gradient = glyphs::gradients[type];
    _base_state->chips_data.push_back(chip);
}

void StoreHandler::changePage(short page_delta) {
    if (_page_index + page_delta < 0 || _page_index + page_delta >= chip_type_range) {
        return;
    } else if (page_delta != 0) {
        _delegate->setIndicatorAlpha(_page_index, ui_disabled_alpha);
        _delegate->didChangePage();
        _page_index += page_delta;
    }
    _delegate->setIndicatorAlpha(_page_index, 1.f);
    _delegate->setBackgroundData(chip_themes[_page_index].gradient, _current_levels[_page_index] ? 0.f : 1.f);
    _delegate->setViewAlpha(1.f);
}

bool StoreHandler::isFirstPage() const {
    return _page_index == 0;
}

bool StoreHandler::isLastPage() const {
    return _page_index == (chip_type_range - 1);
}
