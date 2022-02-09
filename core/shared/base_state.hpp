// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef BASE_STATE_HPP
#define BASE_STATE_HPP

#include <vector>
#include "Chip.hpp"

#define __SERIALIZABLE(type, name)\
    static constexpr const char *key_##name = #name;\
    type name

struct base_state {
    struct shot {
        __SERIALIZABLE(unsigned long , passes) = 0;
        __SERIALIZABLE(unsigned short, cushions_after_last_pass) = 0;
        __SERIALIZABLE(unsigned short, max_cushions_before_pass) = 0;
    };

    __SERIALIZABLE(std::vector<Chip>, chips_data);
    __SERIALIZABLE(shot             , shot_data);
    __SERIALIZABLE(::chip_type      , chip_type) = chip_type_default;
    __SERIALIZABLE(float            , ability_time_left) = 0.f;
    __SERIALIZABLE(float            , charge_level) = 0.f;
    __SERIALIZABLE(unsigned short   , max_continued_game_count) = 0;
    __SERIALIZABLE(unsigned short   , max_supercharge_count) = 0;
    __SERIALIZABLE(unsigned short   , last_index) = 0xFFFF;
    __SERIALIZABLE(bool             , waiting_for_user) = false;

    rect<double> bounds;
    disc         cue {{}, {}, Chip::default_size};
    float        cue_animation_time_s = 0.f;
    bool         cue_visible = false;
};

#endif
