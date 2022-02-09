// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <string>

namespace shaders {
    std::string color_vsh();
    std::string color_fsh();

    std::string gradient_vsh();
    std::string gradient_fsh();

    std::string tint_vsh();
    std::string tint_fsh();

    std::string line_vsh();
    std::string line_fsh();

    std::string circle_vsh();
    std::string circle_fsh();

    std::string texture_vsh();
    std::string texture_fsh();

    std::string overlay_vsh();
    std::string overlay_fsh();

    std::string chip_vsh();
    std::string chip_fsh();

    std::string mask_vsh();

    std::string reduce_overlay_fsh();

    std::string blur_fsh(float scale_factor);
}

#endif
