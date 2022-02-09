// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SOUND_TYPE_HPP
#define SOUND_TYPE_HPP

#include "skill_shot_type.h"

enum sound_type {
    skill_shot_indirect_pass,
    skill_shot_just_passed,
    skill_shot_narrow_gap,
    skill_shot_three_cushion_hit,
    skill_shot_four_passes,
    skill_shot_chips_close,
    skill_shot_two_passes_no_cushion,
    skill_shot_circumvent_chip,
    skill_shot_hit_chips_before_pass,
    achievement_unlocked = skill_shot_type_range,
    goal_completed,
    chip_passed,
    chip_charged,
    chip_hit,
    cushion_hit,
    cue_hit,
    missed,
    sliding
};

#endif
