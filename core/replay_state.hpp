// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef REPLAY_STATE_HPP
#define REPLAY_STATE_HPP

#include <vector>
#include "disc.hpp"

struct replay_state {
    std::vector<vec2<double>> chip_positions;
    disc                      cue;
    double                    speed_coefficient;
};

#endif
