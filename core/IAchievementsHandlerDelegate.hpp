// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IACHIEVEMENTSHANDLERDELEGATE_HPP
#define IACHIEVEMENTSHANDLERDELEGATE_HPP

#include "achievement_type.h"

class IAchievementsHandlerDelegate {
public:
    virtual void achievementUnlocked(achievement_type type) = 0;

    virtual void setAchievementsProgress(const float progress[achievement_type_range]) = 0;
};

#endif
