// Copyright © 2018 Igor Pener. All rights reserved.

#include "shaders.hpp"
#include <sstream>

#define GLSL3(x) "#version 300 es\n" #x

namespace shaders {
    std::string color_vsh() { return
        #include "shaders/color.vsh"
    ;}
    std::string color_fsh() { return
        #include "shaders/color.fsh"
    ;}

    std::string gradient_vsh() { return
        #include "shaders/gradient.vsh"
    ;}
    std::string gradient_fsh() { return
        #include "shaders/gradient.fsh"
    ;}

    std::string tint_vsh() { return
        #include "shaders/tint.vsh"
    ;}
    std::string tint_fsh() { return
        #include "shaders/tint.fsh"
    ;}

    std::string texture_vsh() { return
        #include "shaders/texture.vsh"
    ;}
    std::string texture_fsh() { return
        #include "shaders/texture.fsh"
    ;}

    std::string line_vsh() { return
        #include "shaders/line.vsh"
    ;}
    std::string line_fsh() { return
        #include "shaders/line.fsh"
    ;}

    std::string circle_vsh() { return
        #include "shaders/circle.vsh"
    ;}
    std::string circle_fsh() { return
        #include "shaders/circle.fsh"
    ;}

    std::string overlay_vsh() { return
        #include "shaders/overlay.vsh"
    ;}
    std::string overlay_fsh() { return
        #include "shaders/overlay.fsh"
    ;}

    std::string chip_vsh() { return
        #include "shaders/chip.vsh"
    ;}
    std::string chip_fsh() { return
        #include "shaders/chip.fsh"
    ;}

    std::string mask_vsh() { return
        #include "shaders/mask.vsh"
    ;}

    std::string reduce_overlay_fsh() { return
        #include "shaders/reduce_overlay.fsh"
    ;}

    std::string blur_fsh(float scale_factor) {
        const float base_kernel_size = 18.f;
        int size = static_cast<int>(base_kernel_size * scale_factor + 0.5f);

        if (!(size & 1)) {
            size--;
        }

        double sum = 0.0f;
        double y;
        double *kernel = new double[size];
        double s = static_cast<double>(size);

        // f(x) = 1 - 1/2 * (4x/s - 2)^2 + 1/16 * (4x/s - 2)^4
        for (int i = 0; i < size; ++i) {
            y = 4.0 * (i + 0.5f) / s - 2.0;
            y *= y;
            kernel[i] = 1.0 - 0.5 * y + 0.0625 * y * y;
            sum += kernel[i];
        }
        for (int i = s - 1; i >= 0; --i) {
            kernel[i] /= sum;
        }

        const int half_size = size / 4 + 1;
        double offset[half_size];
        double weights[half_size];
        weights[0] = kernel[size / 2];

        std::stringstream fsh;
        fsh << "#version 300 es\n";
        fsh << "uniform lowp sampler2D sampler;";
        fsh << "uniform mediump vec3 tex_offset;";
        fsh << "in mediump vec2 tex_coord_data;";
        fsh << "layout(location = 0) out lowp vec4 color_data;";
        fsh << "void main() {";
        fsh << "mediump vec3 c = texture(sampler, tex_coord_data).rgb * " << weights[0] << ";";

        for (int i = 1, j = 1; i < half_size; ++i, j += 2) {
            int k = size / 2 + j;
            weights[i] = kernel[k] + kernel[k + 1];
            offset[i - 1] = static_cast<double>(j) * kernel[k];
            offset[i - 1] += static_cast<double>(j + 1) * kernel[k + 1];
            offset[i - 1] /= weights[i];

            fsh << (i - 1 ? "" : "mediump vec2 ") << "d = tex_coord_data + tex_offset.xy * " << offset[i - 1] << ";";
            fsh << (i - 1 ? "" : "mediump vec2 ") << "d_n = tex_coord_data - tex_offset.xy * " << offset[i - 1] << ";";
            fsh << "c += (texture(sampler, d).rgb + texture(sampler, d_n).rgb) * " << weights[i] << ";";
        }

        fsh << "c = (c + (c - sqrt(c.x * c.x * 0.299 + c.y * c.y * 0.587 + c.z * c.z * 0.114)) * 0.1 * tex_offset.z);";
        fsh << "mediump float w = tex_offset.z * 0.2;";
        fsh << "color_data = vec4(c * (1.0 - w) + w, 1.0);";
        fsh << "}";

        delete[] kernel;

        return fsh.str();
    }
}

#undef GLSL3
