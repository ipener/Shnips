// Copyright © 2018 Igor Pener. All rights reserved.

#include "Scene.hpp"

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "ALRenderer.hpp"
#include "assets.hpp"
#include "IAchievementsHandler.hpp"
#include "IGameHandler.hpp"
#include "IRenderer.hpp"
#include "IView.hpp"
#include "Line.hpp"

using namespace std;
using namespace assets;

Scene::Scene(IRenderer *renderer, ALRenderer *al_renderer, base_state *state) : _gl_renderer(renderer), _al_renderer(al_renderer), _state(state) {
    memset(_pass_signaled, 0, sizeof(_pass_signaled));
}

void Scene::init() {
    _max_velocity = 40.f * screen_size.x / 320.f;

    _heart_labels[0] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        30.f,
        glyphs::heart,
        font_weight::thin
    };
    _heart_labels[1] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        30.f,
        glyphs::half_heart,
        font_weight::thin
    };
    _heart_labels[2] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        30.f,
        glyphs::half_heart,
        font_weight::thin
    };

    const auto s = 0.525f;
    const auto w = _heart_labels[0].size.x + _heart_labels[1].size.x * s + _heart_labels[2].size.x * s;

    _heart_labels[0].setPosition({
        (screen_size.x - w + _heart_labels[0].size.x) / 2.f, 0.f
    });
    _heart_labels[1].setPosition({
        _heart_labels[0].p.x + (_heart_labels[0].size.x + _heart_labels[1].size.x * (2.f * s - 1.f)) / 2.f, 0.f
    });
    _heart_labels[2].setPosition({
        _heart_labels[1].p.x + (_heart_labels[1].size.x + _heart_labels[2].size.x * (2.f * s - 1.f)) / 2.f, 0.f
    });
}

void Scene::update(float t) {
    _handler->update(t);

    if (_full_screen_view) {
        _gl_renderer->setZoomData(_full_screen_view->zoomData());
    }

    _background_blend_step += _default_background_blend_step;

    if (_extra_chips_transition_data.size()) {
        _extra_chips_animation_progress += ui_transition_step * t;

        if (_extra_chips_to_be_removed.size()) {
            if (_extra_chips_animation_progress + 1e-5f > 1.f) {
                _extra_chips_to_be_removed.erase(_extra_chips_to_be_removed.begin());
                _extra_chips_transition_data.erase(_extra_chips_transition_data.begin());
                if (_extra_chips_to_be_removed.empty()) {
                    _al_renderer->stopSound<sound_type::sliding>();
                } else {
                    _extra_chips_animation_progress = 0.f;
                }
            }
            if (_extra_chips_to_be_removed.size()) {
                _extra_chips_to_be_removed[0].p = _extra_chips_transition_data[0].initial_p * (1.f - _extra_chips_animation_progress) + _extra_chips_transition_data[0].target_p * _extra_chips_animation_progress;
            }
        } else {
            if (_extra_chips_animation_progress + 1e-5f > 1.f) {
                _state->chips_data[_extra_chips_index].p = _extra_chips_transition_data[0].target_p;
                _extra_chips_transition_data.erase(_extra_chips_transition_data.begin());
                if (_extra_chips_transition_data.empty()) {
                    _al_renderer->stopSound<sound_type::sliding>();
                } else {
                    _extra_chips_index++;
                    _extra_chips_animation_progress = 0.f;
                }
            }
            if (_extra_chips_transition_data.size()) {
                _state->chips_data[_extra_chips_index].p = _extra_chips_transition_data[0].initial_p * (1.f - _extra_chips_animation_progress) + _extra_chips_transition_data[0].target_p * _extra_chips_animation_progress;
            }
        }
    }
    if (_charged_animation_running || fabs(_halo_animation_progress) > 1e-5f) {
        _halo_animation_progress += _halo_animation_step * t;
        if (_halo_animation_progress + 1e-5f > 1.f) {
            _halo_animation_progress = 0.f;
        }
    }
    if (restoreChipsAnimationRunning()) {
        _restore_animation_progress += _restore_animation_transition_step * t;

        auto &p = _state->chips_data[_restore_animation_index].p;
        const auto data = _restore_animation_data[_restore_animation_index];
        if (_restore_animation_progress + 1e-5f > 1.f) {
            _restore_animation_progress = 0.f;
            p = data.target_p;
            if (_restore_animation_index < default_chips_count - 1) {
                _restore_animation_index++;
            } else {
                _restore_animation_index = default_chips_count;
                _handler->onChipsRestored();
                _blend_quad = {};
                _chips_moving = 0;
            }
        } else {
            p = data.initial_p * (1.f - _restore_animation_progress) + data.target_p * _restore_animation_progress;
            return;
        }
    }

    if (_handler->collisionEnabled() && _state->last_index != 0xFFFF) {
        collide(t);
    }
    if (!_chips_moving && _playing_sound) {
        _al_renderer->stopSound<sound_type::sliding>();
        _playing_sound = false;
    }
    achievements_handler->update();

    if (_chips_moving || _state->cue_visible)
        return;

    if (_handler->hit()) {
        if (!_state->shot_data.passes && _handler->playableGame()) {
            _al_renderer->playSound<sound_type::missed>();
        }
        _handler->submit();
        achievements_handler->beginShot();
        return;
    }
}

void Scene::render() {
    float delta = 0.f;
    if (fabs(delta - 1.f) > 1e-5f) {
        if (_background_blend_clear_animation_progress < 1.f) {
            delta = _default_background_blend_clear_step;
            _background_blend_clear_animation_progress += delta;
        } else if (_background_blend_step >= _minimum_background_blend_step) {
            delta = _minimum_background_blend_step;
            _background_blend_step -= delta;
            _background_blend_clear_animation_progress = 1.f;
        }
    }

    _gl_renderer->prepareFrame();
    if (_charge_alpha > 1e-5f) {
        _gl_renderer->drawLine({
             {screen_size.x / 2.f, static_cast<float>(_state->bounds.origin.y + ui_margin)},
             screen_size.x - 2.f * ui_paragraph_margin,
             _charge_alpha
        }, _charge_progress);
    }
    _gl_renderer->prepareLabelDraw();
    if (_heart_labels[0].alpha > 1e-5f) {
        const auto dy = _charge_alpha > 1e-5f ? ui_margin + ui_progress_line_thickness : 0.f;
        const auto y = static_cast<float>(_state->bounds.origin.y + dy + _heart_labels[0].size.y / 2.f);
        for (unsigned short i = 0; i < 3; ++i) {
            _heart_labels[i].setPosition({_heart_labels[i].p.x, y});
        }
        _gl_renderer->drawLabels(_heart_labels);
    }
    _gl_renderer->drawShadows(_state->chips_data);
    if (_state->cue_visible) {
        if (_state->cue_animation_time_s < touch_hide_time_s) {
            const auto size = Chip::default_size * (0.5f + 1.25 * _state->cue_animation_time_s / touch_hide_time_s);
            const auto thickness = (touch_hide_time_s - _state->cue_animation_time_s) * 10.f;
            const float color[4] {0.f, 0.f, 0.f, 0.1f * (touch_hide_time_s - _state->cue_animation_time_s)};
            _gl_renderer->drawHalo(_state->cue.p, size, thickness, color);
        }
        if (_state->cue_animation_time_s > cue_show_time_s) {
            const auto b = cue_show_time_s / (cue_show_time_s - 1.f);
            const auto alpha = min(1.f, (1.f - b) * _state->cue_animation_time_s + b);
            _gl_renderer->drawCue(_state->cue.p, 0.1f * alpha);
        }
    }
    if (fabs(_halo_animation_progress) > 1e-5f) {
        _gl_renderer->prepareCircleDraw();
        const auto size = Chip::default_size * (0.75f + 1.25f * _halo_animation_progress);
        const auto thickness = (1.f - _halo_animation_progress) * 10.f;
        const float color[4] {1.f, 1.f, 1.f, 1.f - _halo_animation_progress};
        for (const auto &chip : _state->chips_data) {
            _gl_renderer->drawHalo(chip.p, size, thickness, color);
        }
    }
    _gl_renderer->drawChips(_state->chips_data);

    if (_extra_chips_to_be_removed.size()) {
        _gl_renderer->drawChips(_extra_chips_to_be_removed);
    }

    _gl_renderer->prepareQuadBlending(delta);

    if (_blend_quad_available) {
        _gl_renderer->drawBlendQuad(_blend_quad.points, _blend_quad.alphas, _blend_quad.white);
        _blend_quad_available = false;
    }
    if (_blend_cushion_hit.p.x > 0.f) {
        _gl_renderer->drawBlendSpot(_blend_cushion_hit.p, _blend_cushion_hit.white, _blend_cushion_hit.alpha);
        _blend_cushion_hit.p.x = -1.f;
    }
    if (_blend_hit.p.x > 0.f) {
        _gl_renderer->drawBlendSpot(_blend_hit.p, _blend_hit.white, _blend_hit.alpha);
        _blend_hit.p.x = -1.f;
    }

    view_type type = view_type::full_screen;
    vec4<float> blur_data;
    if (_alert_view) {
        blur_data = _alert_view->blurData();
        type = _alert_view->type();
    } else if (_full_screen_view) {
        blur_data = _full_screen_view->blurData();
        type = _full_screen_view->type();
    }
    _gl_renderer->setBlurData(type, blur_data);

    _gl_renderer->prepareLabelDraw();
    if (_full_screen_view) {
        _gl_renderer->drawLabels(_full_screen_view->labels());
    }
    if (_alert_view) {
        _gl_renderer->drawLabels(_alert_view->labels());
    }
    _gl_renderer->prepareLineDraw();
    if (_full_screen_view) {
        for (const auto &line : _full_screen_view->lines()) {
            _gl_renderer->drawLine(line);
        }
    }
    if (_alert_view) {
        for (const auto &l : _alert_view->lines()) {
            _gl_renderer->drawLine(l);
        }
    }
    _gl_renderer->prepareCircleDraw();
    if (_full_screen_view) {
        for (const auto &c : _full_screen_view->circles()) {
            _gl_renderer->drawCircle(c, ui_progress_circle_thickness);
        }
    }
    if (_alert_view) {
        for (const auto &c : _alert_view->circles()) {
            _gl_renderer->drawCircle(c, ui_progress_circle_thickness);
        }
    }

    updateSlidingVolume();
}

void Scene::touchesBegan(const vec2<double> &p, long long t_ms) {
    _hit_blank = false;
    _invalid_touch = _chips_moving || restoreChipsAnimationRunning() ||
        _extra_chips_transition_data.size() || _handler->hit();

    if (_invalid_touch)
        return;

    _handler->touchesBegan(p, t_ms);
    achievements_handler->touchesBegan(p, t_ms);

    if (!_handler->interactionAllowed()) {
        _invalid_touch = true;
        return;
    }

    _last_touch_t = t_ms;
    if (_handler->collisionEnabled()) {
        _state->cue.p = p;
        _state->cue.v = {};
        for (const auto &chip : _state->chips_data) {
            if (chip.collides(_state->cue)) {
                _state->cue_visible = false;
                _invalid_touch = true;
                return;
            }
        }
    }

    _touch_velocity = {};
    _last_touch_location = p;
    _state->cue.v = _touch_velocity;
    _state->cue.p = _last_touch_location;
}

void Scene::touchesMoved(const vec2<double> &p, long long t_ms) {
    if (_invalid_touch || restoreChipsAnimationRunning() || _handler->hit())
        return;

    _handler->touchesMoved(p, t_ms);
    achievements_handler->touchesMoved(p, t_ms);

    if (!_chips_moving) {
        _touch_velocity = p - _last_touch_location;
        _last_touch_location = p;
        _state->cue.p = p;
        _state->cue.v = _touch_velocity;
        collideWithCue(getFrameSpeedCoefficient(t_ms), false);
        _last_touch_t = t_ms;
    }
}

void Scene::touchesEnded(const vec2<double> &p, long long t_ms) {
    if (_invalid_touch || restoreChipsAnimationRunning() || _handler->hit())
        return;

    if (_hit_blank) {
        _handler->hit(0xFFFF);
    }
    if (!_chips_moving) {
        _touch_velocity = p - _last_touch_location;
        _state->cue.p = p;
        _state->cue.v = _touch_velocity;
        collideWithCue(getFrameSpeedCoefficient(t_ms), false);
    }

    _handler->touchesEnded(p, t_ms);
    achievements_handler->touchesEnded(p, t_ms);
}

void Scene::setChargeAlpha(float alpha) {
    _charge_alpha = alpha;
}

void Scene::setHeartAlpha(unsigned short index, float alpha) {
    _heart_labels[index % 3].alpha = alpha;
}

void Scene::setGameHandler(IGameHandler *handler) {
    _restore_animation_index = default_chips_count;
    if (_handler != nullptr) {
        _handler->willResignActive();
        if (!_charged_animation_running) {
            _halo_animation_progress = 0.f;
        }
    }
    _chips_moving = 0;
    _handler = handler;
    _handler->willBecomeActive();

    memset(_pass_signaled, 0, sizeof(_pass_signaled));
}

void Scene::setFullScreenView(const IView *view) {
    _full_screen_view = view;
}

void Scene::setAlertView(const IView *view) {
    _alert_view = view;
}

void Scene::setBackgroundBlending(bool enabled) {
    _blending_enabled = enabled;
}

bool Scene::chipsMoving() const {
    return _chips_moving;
}

void Scene::collideWithCue() {
    collideWithCue(1.f, true);
}

void Scene::addExtraChips(unsigned short n) {
    if (!n || _state->chips_data.size() != default_chips_count)
        return;

    _al_renderer->playSound<sound_type::sliding>();
    _extra_chips_index = default_chips_count;
    _extra_chips_animation_progress = 0.f;
    _extra_chips_transition_data.clear();
    const auto d = Chip::default_size + 5.0;
    const auto offset = Chip::default_size * 2.0;
    const double edges[4] = {-offset, -offset, screen_size.x + offset, screen_size.y + offset};
    vec2<double> q;
    Chip chip = _state->chips_data[0];
    chip.icon = glyphs::blank_chip;

    for (unsigned short i = 0, j = 0; i < 1000 && j < n; ++i) {
        const auto e = rand() % 4;
        if (e & 1) {
            chip.p.y = edges[e];
            chip.p.x = static_cast<double>(rand() % static_cast<int>(_state->bounds.size.x)) + _state->bounds.origin.x;
        } else {
            chip.p.x = edges[e];
            chip.p.y = static_cast<double>(rand() % static_cast<int>(_state->bounds.size.y)) + _state->bounds.origin.y;
        }
        q.x = rand() % static_cast<int>(_state->bounds.size.x - 2.0 * d) + d + _state->bounds.origin.x;
        q.y = rand() % static_cast<int>(_state->bounds.size.y - 2.0 * d) + d + _state->bounds.origin.y;
        chip.v = q - chip.p;

        bool found = true;
        for (const auto &c : _state->chips_data) {
            if (c.p.dist(q) < 1.0 + offset * 2.0 || chip.timeToCollision(&c) + 1e-9 < 1.0) {
                found = false;
                break;
            }
        }
        chip.v = {};

        if (found) {
            _extra_chips_transition_data.push_back({chip.p, q});
            chip.p = q;
            _state->chips_data.push_back(chip);
            j++;
        }
    }
    for (unsigned short i = 0; i < n; ++i) {
        _state->chips_data[_extra_chips_index + i].p = _extra_chips_transition_data[i].initial_p;
    }
}

void Scene::removeExtraChips() {
    if (_state->chips_data.size() > default_chips_count) {
        _al_renderer->playSound<sound_type::sliding>();
        _extra_chips_animation_progress = 0.f;
        _extra_chips_to_be_removed.clear();
        _extra_chips_transition_data.clear();
        if (_state->last_index < 2) {
            swap(_state->chips_data[_state->last_index], _state->chips_data[2]);
            _state->last_index = 2;
        }
        const auto size = _state->chips_data.size() - default_chips_count;
        for (unsigned short i = 0; i < size; ++i) {
            removeOneBlankChip();
        }
    }
}

void Scene::restoreChips(vector<vec2<double>> positions, unsigned short last_index, bool clear_background) {
    if (restoreChipsAnimationRunning() || _state->chips_data.size() < default_chips_count)
        return;

    if (clear_background) {
        _background_blend_clear_animation_progress = 0.f;
    }

    const rect<double> bounds({0.0, 0.0}, {screen_size.x, screen_size.y});
    if (positions.size() < default_chips_count) {
        Chip::defaultChipPositions(default_chips_count, bounds, [&positions](const vec2<double> &p) {
            positions.push_back(p);
        });
    }

    const size_t permutations[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    double best_d = 1e100;
    int chips_permutation = -1;
    int closest_permutation = -1;
    if (default_chips_count >= 3) {
        for (int chips_p = 0; chips_p < 6; ++chips_p) {
            for (int closest_p = 0; closest_p < 6; ++closest_p) {
                double curr_d = 0;
                bool collision = false;
                for (int i = 0; i < 3 && !collision; ++i) {
                    if (last_index != 0xFFFF && permutations[chips_p][i] == _state->last_index && permutations[closest_p][i] != last_index) {
                        collision = true;
                    }
                    curr_d += _state->chips_data[permutations[chips_p][i]].p.dist(positions[permutations[closest_p][i]]);
                    for (int matched = 0; matched < i; ++matched) {
                        vec2<double> potential_collision = positions[permutations[closest_p][matched]];
                        if (_state->chips_data[permutations[chips_p][i]].collidesOnTranslation(potential_collision, positions[permutations[closest_p][i]])) {
                            collision = true;
                            break;
                        }
                    }
                    for (int unmatched = i+1; unmatched < 3; ++unmatched) {
                        vec2<double> potential_collision = _state->chips_data[permutations[chips_p][unmatched]].p;
                        if (_state->chips_data[permutations[chips_p][i]].collidesOnTranslation(potential_collision, positions[permutations[closest_p][i]])) {
                            collision = true;
                            break;
                        }
                    }
                }
                if (!collision && curr_d < best_d) {
                    best_d = curr_d;
                    for (int i = 0; i < 3; ++i) {
                        chips_permutation = chips_p;
                        closest_permutation = closest_p;
                    }
                }
            }
        }
    }

    size_t closest[default_chips_count];
    if (chips_permutation != -1 && default_chips_count == 3) {
        for (int i = 0; i < 3; ++i) {
            if (permutations[chips_permutation][i] == _state->last_index) {
                _state->last_index = i;
                break;
            }
        }
        _state->chips_data = {
            _state->chips_data[permutations[chips_permutation][0]],
            _state->chips_data[permutations[chips_permutation][1]],
            _state->chips_data[permutations[chips_permutation][2]]
        };
        for (size_t i = 0; i < default_chips_count; ++i) {
            closest[i] = permutations[closest_permutation][i];
        }
    } else {
        for (size_t i = 0; i < default_chips_count; ++i) {
            closest[i] = i;
        }
    }

    _restore_animation_index = 0;
    for (unsigned short i = 0; i < default_chips_count; ++i) {
        _restore_animation_data[i] = {_state->chips_data[i].p, positions[closest[i]]};
    }
}

void Scene::setChipsSpeedCoefficient(float speed_coeff) {
    _chips_speed_coefficient = speed_coeff;
}

void Scene::setChargedAnimation(bool start, bool play_sound) {
    _charged_animation_running = start;
    if (play_sound) {
        _al_renderer->playSound<sound_type::chip_charged>();
    }
}

void Scene::setChargeProgress(float progress) {
    _charge_progress = progress;
}

bool Scene::willPass(base_state &state) const {
    const float time_difference = 20.f;
    const auto size = state.chips_data.size();
    double dt = fabs(time_difference) < 1e-9 ? 0.0 : time_difference / seconds_for_60_fps;

    while (dt > 1e-6) {
        double t = dt;
        t = min(t, timeUntilCollision(state));

        for (size_t i = 0; i < size - 1; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
                const auto &ci = state.chips_data[i];
                const auto &cj = state.chips_data[j];
                const auto &cl = state.chips_data[state.last_index];
                if (i != state.last_index && j != state.last_index) {
                    double t_until_pass = cl.timeUntilPass(&ci, &cj);
                    if (t_until_pass - 1e-5 < t) {
                        return true;
                    }
                }
            }
        }
        unsigned short chips_moving = 0;
        updateScene(state, t, chips_moving);
        if (!chips_moving) {
            return false;
        }

        for (size_t i = 0; i < size - 1; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
                state.chips_data[i].collide(&state.chips_data[j]);
            }
        }
        for (size_t i = 0; i < size; ++i) {
            state.chips_data[i].collide(_state->bounds);
        }
        dt -= t;
    }
    return false;
}

void Scene::skillShotAchieved(skill_shot_type type) {
    _al_renderer->playSound(type);
}

void Scene::removeOneBlankChip() {
    const double offset = Chip::default_size * 2.f;
    double edge_distances[4];
    for (unsigned short i = _state->chips_data.size() - 1; i >= 2; --i) {
        const auto v_0_1 = _state->chips_data[0].p - _state->chips_data[1].p;
        const auto s = v_0_1.dot(_state->chips_data[i].p) / v_0_1.normSquared();
        const auto v_i = v_0_1 * s - _state->chips_data[i].p;

        edge_distances[0] = (-offset - _state->chips_data[i].p.x) / v_i.x;
        edge_distances[1] = (-offset - _state->chips_data[i].p.y) / v_i.y;
        edge_distances[2] = (screen_size.x + offset - _state->chips_data[i].p.x) / v_i.x;
        edge_distances[3] = (screen_size.y + offset - _state->chips_data[i].p.y) / v_i.y;
        sort(edge_distances, edge_distances + 4, [](double a, double b) -> bool {
            return fabs(a) < fabs(b);
        });

        for (unsigned short j = 0; j < 4; ++j) {
            const auto v = v_i * edge_distances[j];
            bool no_collision = true;
            _state->chips_data[i].v = v;
            for (unsigned short k = 0; k < _state->chips_data.size(); ++k) {
                if (k != i && _state->chips_data[i].timeToCollision(&_state->chips_data[k]) + 1e-9 < 1.0) {
                    no_collision = false;
                    break;
                }
            }
            _state->chips_data[i].v = {};

            if (no_collision) {
                _extra_chips_to_be_removed.push_back(_state->chips_data[i]);
                _extra_chips_transition_data.push_back({
                    _state->chips_data[i].p, _state->chips_data[i].p + v
                });
                _state->chips_data.erase(_state->chips_data.begin() + i);
                return;
            }
        }
    }
}

void Scene::collideWithCue(double frame_speed_coefficient, bool ignore_chip_speed_coeff) {
    if (!_handler->collisionEnabled())
        return;

    const auto size = _state->chips_data.size();
    double t = 1e50;
    unsigned short j = 0xFFFF;
    for (unsigned short i = 0; i < size; ++i) {
        if (i != _state->last_index && _state->chips_data[i].icon != glyphs::blank_chip) {
            double s = _state->chips_data[i].timeToCollision(_state->cue);
            if (s < t) {
                _hit_blank = false;
                t = s;
                j = i;
            }
        } else if (_state->chips_data[i].timeToCollision(_state->cue) < 1e50) {
            _hit_blank = true;
        }
    }

    if (j != 0xFFFF) {
        _state->cue.p += _state->cue.v * t;
        _state->chips_data[j].collideWithCue(_state->cue);

        const double d = _state->cue.v.norm() * frame_speed_coefficient;
        if (d > _max_velocity) {
            _state->chips_data[j].v *= _max_velocity / d;
        }
        if (!ignore_chip_speed_coeff) {
            _state->chips_data[j].v *= _chips_speed_coefficient * frame_speed_coefficient;
        } else {
            _state->chips_data[j].v *= frame_speed_coefficient;
        }

        _al_renderer->playSound<sound_type::sliding>();
        _al_renderer->playSound<sound_type::cue_hit>();
        _playing_sound = true;
        _invalid_touch = true;
        if (_state->last_index < default_chips_count) {
            const auto type = _state->chips_data[_state->last_index].type;
            _state->chips_data[_state->last_index].icon = glyphs::chips[type];
        }
        _state->cue_visible = false;
        _state->last_index = j;
        _state->chips_data[_state->last_index].icon = glyphs::blank_chip;
        achievements_handler->endShot();
        _invalid_touch = true;
        _handler->hit(j);
        if (_blending_enabled) {
            _blend_hit.p.x = _state->chips_data[j].p.x;
            _blend_hit.p.y = _state->chips_data[j].p.y;
        }
        memset(_pass_signaled, 0, sizeof(_pass_signaled));
    }
}

void Scene::collide(float time_difference) {
    const auto size = _state->chips_data.size();
    double dt = fabs(time_difference) < 1e-9 ? 0.0 : time_difference / seconds_for_60_fps;
    bool potential_pass;

    while (dt > 1e-6) {
        double t = min(dt, timeUntilCollision(*_state));

        // Check for a pass
        if (_handler->hit()) {
            potential_pass = false;
            for (size_t i = 0; i < size - 1; ++i) {
                for (size_t j = i + 1; j < size; ++j) {
                    const auto &ci = _state->chips_data[i];
                    const auto &cj = _state->chips_data[j];
                    const auto &cl = _state->chips_data[_state->last_index];
                    if (i != _state->last_index && j != _state->last_index && !_pass_signaled[i][j]) {
                        double t_until_pass = cl.timeUntilPass(&ci, &cj);
                        if (t_until_pass - 1e-5 < t) {
                            potential_pass = true;
                            _should_pass[i][j] = true;
                            _cross_product[i][j] = (cj.p - ci.p).cross(cl.p - ci.p);
                        } else {
                            _should_pass[i][j] = false;
                        }
                    }
                }
            }
        } else {
            potential_pass = false;
        }

        updateScene(*_state, t, _chips_moving);

        // Confirm pass
        if (potential_pass) {
            for (size_t i = 0; i < size - 1; ++i) {
                for (size_t j = i + 1; j < size; ++j) {
                    const auto &ci = _state->chips_data[i];
                    const auto &cj = _state->chips_data[j];
                    const auto &cl = _state->chips_data[_state->last_index];
                    if (i != _state->last_index && j != _state->last_index) {
                        if (_should_pass[i][j] && !_pass_signaled[i][j]) {
                            const double new_cross_product = (cj.p - ci.p).cross(cl.p - ci.p);
                            if (_cross_product[i][j] * new_cross_product < 1e-6 || fabs(new_cross_product) < 1e-4 || fabs(_cross_product[i][j]) < 1e-4) {
                                _pass_signaled[i][j] = true;
                                passed(i, j);
                            }
                        }
                    }
                }
            }
        }

        // Collide
        for (size_t i = 0; i < size - 1; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
                if (_state->chips_data[i].collide(&_state->chips_data[j])) {
                    _al_renderer->playSound<sound_type::chip_hit>();
                    memset(_pass_signaled, 0, sizeof(_pass_signaled));
                    if (i == _state->last_index) {
                        _handler->chipHit(j);
                    } else if (j == _state->last_index) {
                        _handler->chipHit(i);
                    }
                }
            }
        }

        for (size_t i = 0; i < size; ++i) {
            if (_state->chips_data[i].collide(_state->bounds)) {
                memset(_pass_signaled, 0, sizeof(_pass_signaled));
                if (i == _state->last_index) {
                    auto p = _state->chips_data[i].p;

                    if (p.x - _state->chips_data[i].r < _state->bounds.origin.x + 1e-9) {
                        p.x = _state->bounds.origin.x;
                    }
                    if (p.x + _state->chips_data[i].r > _state->bounds.origin.x + _state->bounds.size.x - 1e-9) {
                        p.x = _state->bounds.origin.x + _state->bounds.size.x;
                    }
                    if (p.y - _state->chips_data[i].r < _state->bounds.origin.y + 1e-9) {
                        p.y = _state->bounds.origin.y;
                    }
                    if (p.y + _state->chips_data[i].r > _state->bounds.origin.y + _state->bounds.size.y - 1e-9) {
                        p.y = _state->bounds.origin.y + _state->bounds.size.y;
                    }
                    if (_blending_enabled) {
                        _blend_cushion_hit.p.x = p.x;
                        _blend_cushion_hit.p.y = p.y;
                    }
                    _handler->cushionHit();
                }
                _al_renderer->playSound<sound_type::cushion_hit>();
            }
        }

        dt -= t;
    }
}

double Scene::timeUntilCollision(const base_state &state) const {
    double t = 1e100;
    const auto size = state.chips_data.size();
    for (size_t i = 0; i < size; ++i) {
        t = min(t, state.chips_data[i].timeToCollision(_state->bounds));
        for (size_t j = i + 1; j < size; ++j) {
            t = min(t, state.chips_data[i].timeToCollision(&state.chips_data[j]));
        }
    }
    return t;
}

void Scene::updateScene(base_state &state, double t, unsigned short &chips_moving) const {
    for (size_t i = 0; i < state.chips_data.size(); ++i) {
        if (state.chips_data[i].v != 0.0) {
            const double current_v = state.chips_data[i].v.norm();
            const vec2<double> v = state.chips_data[i].v / current_v;
            double final_t = t;
            double final_v = current_v - assets::deceleration_factor * t;
            if (final_v < 0.0) {
                final_t = current_v / assets::deceleration_factor;
                final_v = 0.0;
            }
            state.chips_data[i].p += v * (current_v + final_v) * 0.5 * final_t;
            state.chips_data[i].v = v * final_v;
            state.chips_data[i].v == 0.0 ? chips_moving &= ~(1 << i) : chips_moving |= (1 << i);
        } else {
            chips_moving &= ~(1 << i);
        }
    }
}

void Scene::passed(unsigned short i, unsigned short j) {
    _handler->passed(i, j);
    _al_renderer->playSound<sound_type::chip_passed>();

    if (_blending_enabled) {
        blend_quad q = _blend_quad;
        computeBlendQuadVertices(_blend_quad.points, _state->chips_data[i].p, _state->chips_data[j].p);

        if (q.points[0] != _blend_quad.points[0] || q.points[1] != _blend_quad.points[1] ||
            q.points[2] != _blend_quad.points[2] || q.points[3] != _blend_quad.points[3]) {
            const auto v = static_cast<float>(_state->chips_data[_state->last_index].v.normSquared() / (40.0 * 40.0));
            if (rand() % 2) {
                _blend_quad.white = true;
                for (short k = 0; k < 4; ++k) {
                    _blend_quad.alphas[k] = static_cast<float>(rand() % 5) / 25.f + v * 0.1f + 0.05f;
                }
            } else {
                _blend_quad.white = false;
                for (short k = 0; k < 4; ++k) {
                    _blend_quad.alphas[k] = static_cast<float>(rand() % 5) / 25.f + v * 0.09f + 0.01f;
                }
            }
            _blend_quad_available = true;
        } else {
            _blend_quad_available = false;
        }
    }
}

void Scene::updateSlidingVolume() {
    if (_extra_chips_transition_data.size()) {
        _al_renderer->setVolume<sound_type::sliding>(1.f - _extra_chips_animation_progress);
    } else {
        float volume = 0.f;
        for (const auto &chip: _state->chips_data) {
            volume += fabs(chip.v.x) + fabs(chip.v.y);
        }
        volume /= 20.f;
        _al_renderer->setVolume<sound_type::sliding>(volume > 1.f ? 1.f : volume);
    }
}

void Scene::computeBlendQuadVertices(vec2<float> points[4], const vec2<double> &p, const vec2<double> &q) {
    unsigned short mask = 0;
    if (fabs(p.x - q.x) > 1e-5) {
        const double m = (p.y - q.y) / (p.x - q.x);
        const double b = p.y - m * p.x;
        double u = b + m * screen_size.x;

        if (u > -1e-5 && u < screen_size.y + 1e-5) {
            points[0] = {screen_size.x, static_cast<float>(u)};
            mask |= 1;
        }
        if (b > -1e-5 && b < screen_size.y + 1e-5) {
            points[mask ? 1 : 0] = {0.f, static_cast<float>(b)};
            mask |= 4;
        }
        if (mask < 5 && fabs(m) > 1e-5) {
            u = -b / m;
            if (u > -1e-5 && u < screen_size.x + 1e-5) {
                points[mask ? 1 : 0] = {static_cast<float>(u), 0.f};
                mask |= 2;
            }
            u = (screen_size.y - b) / m;
            if (u > -1e-5 && u < screen_size.x + 1e-5) {
                points[mask ? 1 : 0] = {static_cast<float>(u), screen_size.y};
                mask |= 8;
            }
        }
    } else {
        points[0] = {static_cast<float>(p.x), 0.f};
        points[2] = {static_cast<float>(p.x), screen_size.y};
        mask = 10;
    }
    if (mask == 5) {
        if (points[0].y + points[1].y < screen_size.y) {
            points[2] = {screen_size.x, 0.f};
            points[3] = {0.f, 0.f};
        } else {
            points[2] = {screen_size.x, screen_size.y};
            points[3] = {0.f, screen_size.y};
        }
    } else if (mask == 10) {
        if (points[0].x + points[1].x < screen_size.x) {
            points[2] = {0.f, 0.f};
            points[3] = {0.f, screen_size.y};
        } else {
            points[2] = {screen_size.x, 0.f};
            points[3] = {screen_size.x, screen_size.y};
        }
    } else if (mask == 3) {
        points[2] = points[3] = {screen_size.x, 0.f};
    } else if (mask == 6) {
        points[2] = points[3] = {0.f, 0.f};
    } else if (mask == 12) {
        points[2] = points[3] = {0.f, screen_size.y};
    } else if (mask == 9) {
        points[2] = points[3] = {screen_size.x, screen_size.y};
    }
}

double Scene::getFrameSpeedCoefficient(long long t) {
    return t - _last_touch_t > 0 ? seconds_for_60_fps / ((t - _last_touch_t) / 1000.0) : 1.0;
}

bool Scene::restoreChipsAnimationRunning() const {
    return _restore_animation_index < default_chips_count;
}

