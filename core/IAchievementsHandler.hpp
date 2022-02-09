// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IACHIEVEMENTSHANDLER_HPP
#define IACHIEVEMENTSHANDLER_HPP

#include "app_state.hpp"
#include "skill_shot_type.h"

class IAchievementsHandler {
public:
    virtual ~IAchievementsHandler() {}

    virtual void tutorialCompleted() = 0;

    virtual void submit() = 0;

    virtual void beginShot() = 0;

    virtual void endShot() = 0;

    virtual void update() = 0;

    virtual void heartUsed() = 0;

    virtual void goalCompleted(chip_type unlocked) = 0;

    virtual void skillShotAchieved(skill_shot_type type) = 0;

    virtual void supercharged() = 0;

    virtual void chipUpgraded(chip_type type, unsigned short level) = 0;

    virtual void touchesBegan(const vec2<double> &p, long long t_ms) = 0;

    virtual void touchesMoved(const vec2<double> &p, long long t_ms) = 0;

    virtual void touchesEnded(const vec2<double> &p, long long t_ms) = 0;
};

extern IAchievementsHandler *achievements_handler;

#endif
