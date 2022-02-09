// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ISINGLEPLAYERSCENEDELEGATE_HPP
#define ISINGLEPLAYERSCENEDELEGATE_HPP

#include "rect.hpp"
#include "skill_shot_type.h"

class ISingleplayerSceneDelegate {
public:
    virtual void addExtraChips(unsigned short n) = 0;

    virtual void removeExtraChips() = 0;

    virtual void restoreChips(std::vector<vec2<double>> positions, unsigned short last_index = 0xFFFF, bool clear_background = true) = 0;

    virtual void setChipsSpeedCoefficient(float speed_coeff) = 0;

    virtual void setChargedAnimation(bool start, bool play_sound = false) = 0;

    virtual void setChargeProgress(float progress) = 0;

    virtual bool willPass(base_state &chips) const = 0;

    virtual void skillShotAchieved(skill_shot_type type) = 0;
};

#endif
