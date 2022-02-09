// Copyright © 2018 Igor Pener. All rights reserved.

#include "SkillShotHandler.hpp"

#include "assets.hpp"
#include "base_state.hpp"
#include "Logger.hpp"

using namespace assets;

SkillShotHandler::SkillShotHandler() {
    _points[skill_shot_type_indirect_pass] = 3;
    _points[skill_shot_type_just_passed] = 2;
    _points[skill_shot_type_narrow_gap] = 2;
    _points[skill_shot_type_three_cushion_hit] = 2;
    _points[skill_shot_type_four_passes] = 3;
    _points[skill_shot_type_chips_close] = 3;
    _points[skill_shot_type_two_passes_no_cushion] = 4;
    _points[skill_shot_type_circumvent_chip] = 5;
    _points[skill_shot_type_hit_chips_before_pass] = 4;
    SkillShot::delegate = this;
}

void SkillShotHandler::init(const base_state *state, ISkillShotHandlerDelegate *delegate) {
    SkillShot::state = state;
    _delegate = delegate;
}

void SkillShotHandler::setEnabled(bool enabled) {
    _enabled = enabled;
}

void SkillShotHandler::passed(unsigned short i, unsigned short j) {
    if (_enabled) {
        _indirect_pass_skill_shot.passed();
        _just_passed_skill_shot.passed(i, j);
        _narrow_gap_skill_shot.passed(i, j);
        _three_cushion_hit_skill_shot.passed();
        _four_passes_skill_shot.passed();
        _chips_close_skill_shot.passed();
        _two_passes_no_cushion_skill_shot.passed();
        _circumvent_chip_skill_shot.passed();
        _hit_chips_before_pass_skill_shot.passed();
    }
}

void SkillShotHandler::hit() {
    if (_enabled) {
        _chips_close_skill_shot.hit();
        _two_passes_no_cushion_skill_shot.hit();
        _circumvent_chip_skill_shot.hit();
        _hit_chips_before_pass_skill_shot.hit();
    }
}

void SkillShotHandler::chipHit(unsigned short i) {
    if (_enabled) {
        _hit_chips_before_pass_skill_shot.hit(i);
    }
}

void SkillShotHandler::cushionHit() {
    if (_enabled) {
        _three_cushion_hit_skill_shot.cushionHit();
        _two_passes_no_cushion_skill_shot.cushionHit();
    }
}

void SkillShotHandler::submit() {
    if (_enabled) {
        _indirect_pass_skill_shot.submit();
        _just_passed_skill_shot.submit();
        _narrow_gap_skill_shot.submit();
        _three_cushion_hit_skill_shot.submit();
        _four_passes_skill_shot.submit();
        _two_passes_no_cushion_skill_shot.submit();
    }
}

void SkillShotHandler::update(float t) {
    if (_enabled) {
        _circumvent_chip_skill_shot.update();
    }

    if (!_notifications.empty()) {
        _display_time_left -= t;
        if (_display_time_left < 1e-5f) {
            _display_time_left = 0.f;
            _notification_alpha = 0.f;
            _notifications.pop();
            if (!_notifications.empty()) {
                _display_time_left = info_display_time_s;
                _delegate->setNotification(_notifications.front());
            }
        } else if (_display_time_left < 0.5f) {
            _notification_alpha -= t * ui_transition_step;
            if (_notification_alpha < 0.f) {
                _notification_alpha = 0.f;
            }
        } else if (_display_time_left > info_display_time_s - 0.5f) {
            _notification_alpha += t * ui_transition_step;
            if (_notification_alpha > 1.f) {
                _notification_alpha = 1.f;
            }
        }
    }
}

void SkillShotHandler::dismissNotifications() {
    while (!_notifications.empty()) {
        _notifications.pop();
    }
    _display_time_left = 0.f;
    _notification_alpha = 0.f;
}

float SkillShotHandler::notificationAlpha() const {
    return _notification_alpha;
}

unsigned short SkillShotHandler::pointsForSkillShot(skill_shot_type type) {
    return _points[type];
}

void SkillShotHandler::skillShotAchieved(skill_shot_type type) {
    if (_enabled) {
        _delegate->skillShotAchieved(type);
        if (_notifications.empty()) {
            _display_time_left = info_display_time_s;
            _notification_alpha = 0.f;
            _delegate->setNotification(type);
        }
        _notifications.push(type);
    }
}
