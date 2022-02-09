// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

#include "base_state.hpp"
#include "chip_type.h"
#include "game_type.hpp"

// Note: Never rename these keys!
__SERIALIZABLE(struct, app_state) {
    __SERIALIZABLE(struct, singleplayer) {
        __SERIALIZABLE(unsigned long long, score) = 0;
        __SERIALIZABLE(unsigned long long, highscore) = 0;
        __SERIALIZABLE(unsigned long long, progress) = 0;
        __SERIALIZABLE(unsigned long     , supercharge_count) = 0; // overall_count << 2 + per_game_count
        __SERIALIZABLE(::game_type       , game_type) = ::game_type::highscore;
        __SERIALIZABLE(unsigned short    , goal) = 0;
        __SERIALIZABLE(unsigned short    , continued_game_count) = 0;
        __SERIALIZABLE(bool              , charging_intro_seen) = false;
        __SERIALIZABLE(bool              , swap_intro_seen) = false;
    };

    __SERIALIZABLE(struct, store) {
        __SERIALIZABLE(std::vector<unsigned long>, progress);
    };

    __SERIALIZABLE(struct, tutorial) {
        __SERIALIZABLE(bool, completed) = false;
    };

    __SERIALIZABLE(struct, achievements) {
        __SERIALIZABLE(std::vector<unsigned long>, progress);
    };

    __SERIALIZABLE(::base_state  , base_state);
    __SERIALIZABLE(unsigned short, rate_app_prompts_count) = 0;
    __SERIALIZABLE(bool          , muted) = false;
    __SERIALIZABLE(bool          , discard_base_state) = false;
    __SERIALIZABLE(bool          , followed_on_twitter) = false;
};

#endif
