// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ISKILLSHOTDELEGATE_HPP
#define ISKILLSHOTDELEGATE_HPP

#include "skill_shot_type.h"

class ISkillShotDelegate {
public:
    virtual void skillShotAchieved(skill_shot_type type) = 0;
};

#endif
