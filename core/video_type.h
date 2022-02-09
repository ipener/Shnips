// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef VIDEO_TYPE_H
#define VIDEO_TYPE_H

#include "skill_shot_type.h"

typedef enum {
    video_type_indirect_pass         = skill_shot_type_indirect_pass,
    video_type_just_passed           = skill_shot_type_just_passed,
    video_type_narrow_gap            = skill_shot_type_narrow_gap,
    video_type_three_cushion_hit     = skill_shot_type_three_cushion_hit,
    video_type_four_passes           = skill_shot_type_four_passes,
    video_type_chips_close           = skill_shot_type_chips_close,
    video_type_two_passes_no_cushion = skill_shot_type_two_passes_no_cushion,
    video_type_circumvent_chip       = skill_shot_type_circumvent_chip,
    video_type_hit_chips_before_pass = skill_shot_type_hit_chips_before_pass,
    video_type_no_cushion_shot       = skill_shot_type_range,
    video_type_double_cushion_shot,
    video_type_double_pass,
    video_type_triple_pass,
    video_type_range
} video_type;

#endif
