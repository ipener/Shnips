// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IVIDEOSCENEDELEGATE_HPP
#define IVIDEOSCENEDELEGATE_HPP

#include <vector>
#include "vec.hpp"

class IVideoSceneDelegate {
public:
    virtual void collideWithCue() = 0;

    virtual void restoreChips(std::vector<vec2<double>> positions, unsigned short last_index = 0xFFFF, bool clear_background = true) = 0;
};

#endif
