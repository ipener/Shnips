// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SKILLSHOTHANDLER_HPP
#define SKILLSHOTHANDLER_HPP

#include <queue>
#include "skill_shot_type.h"
#include "SkillShots.hpp"

class ISkillShotHandlerDelegate : public ISkillShotDelegate {
public:
    virtual void setNotification(skill_shot_type type) = 0;
};

class SkillShotHandler : public ISkillShotDelegate {
public:
    SkillShotHandler();
    
    void init(const struct base_state *state, ISkillShotHandlerDelegate *delegate);

    void setEnabled(bool enabled);

    void passed(unsigned short i, unsigned short j);

    void hit();

    void chipHit(unsigned short i);

    void cushionHit();

    void submit();
    
    void update(float t);

    void dismissNotifications();

    float notificationAlpha() const;

    unsigned short pointsForSkillShot(skill_shot_type type);

private:
    void skillShotAchieved(skill_shot_type type) override;

private:
    ISkillShotHandlerDelegate  *_delegate = nullptr;

    IndirectPassSkillShot       _indirect_pass_skill_shot;
    JustPassedSkillShot         _just_passed_skill_shot;
    NarrowGapSkillShot          _narrow_gap_skill_shot;
    ThreeCushionHitSkillShot    _three_cushion_hit_skill_shot;
    FourPassSkillShot           _four_passes_skill_shot;
    ChipsCloseSkillShot         _chips_close_skill_shot;
    TwoPassesNoCushionSkillShot _two_passes_no_cushion_skill_shot;
    CircumventChipSkillShot     _circumvent_chip_skill_shot;
    HitChipsBeforePassSkillShot _hit_chips_before_pass_skill_shot;

    std::queue<skill_shot_type> _notifications;

    unsigned short              _points[skill_shot_type_range];
    float                       _notification_alpha = 0.f;
    float                       _display_time_left = 0.f;
    bool                        _enabled = false;
};

#endif
