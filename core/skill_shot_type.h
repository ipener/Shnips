// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SKILL_SHOT_TYPE_H
#define SKILL_SHOT_TYPE_H

// To avoid screwing up the achieve_all_skill_shots achievement,
// add new achievement types only at the bottom.
typedef enum {
    skill_shot_type_indirect_pass,
    skill_shot_type_just_passed,
    skill_shot_type_narrow_gap,
    skill_shot_type_three_cushion_hit,
    skill_shot_type_four_passes,
    skill_shot_type_chips_close,
    skill_shot_type_two_passes_no_cushion,
    skill_shot_type_circumvent_chip,
    skill_shot_type_hit_chips_before_pass,
    skill_shot_type_range
} skill_shot_type;

#endif
