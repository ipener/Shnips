// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ITUTORIALSCENEDELEGATE_HPP
#define ITUTORIALSCENEDELEGATE_HPP

#include <vector>
#include "vec.hpp"

class ITutorialSceneDelegate {
public:
    virtual void restoreChips(std::vector<vec2<double>> positions, unsigned short last_index = 0xFFFF, bool clear_background = true) = 0;
};

#endif
