// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ACHIEVEMENT_TYPE_H
#define ACHIEVEMENT_TYPE_H

// To avoid screwing up the state, add new achievement types only at the bottom.
// Do not change the names because they are immutable in App Store and Google Play.
typedef enum {
    achievement_type_tutorial_completed,
    achievement_type_successful_shots,
    achievement_type_cushion_shots,
    achievement_type_no_cushion_shots,
    achievement_type_consecutive_shots,
    achievement_type_consecutive_fast_shots,
    achievement_type_chip_type_fast_unlocked,
    achievement_type_chip_type_extra_points_unlocked,
    achievement_type_chip_type_swap_unlocked,
    achievement_type_chip_type_extra_chip_unlocked,
    achievement_type_goals_completed,
    achievement_type_played_every_day,
    achievement_type_half_screen_play,
    achievement_type_lose_at_666,
    achievement_type_sos,
    achievement_type_all_chips_touching_cushions,
    achievement_type_smiley,
    achievement_type_all_skill_shots_achieved,
    achievement_type_hearts_earned,
    achievement_type_hearts_used,
    achievement_type_supercharged,
    achievement_type_chip_type_fast_upgraded,
    achievement_type_chip_type_extra_points_upgraded,
    achievement_type_chip_type_swap_upgraded,
    achievement_type_chip_type_extra_chip_upgraded,
    achievement_type_score_points,
    achievement_type_range
} achievement_type;

#endif
