// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <vector>
#include <unordered_map>

#include "button_type.h"
#include "chip_type.h"
#include "IResourceLoader.hpp"
#include "rect.hpp"

#define M_PI_32 0.09817477042f
#define M_PI_64 0.04908738521f

typedef struct {
    const float tint_color;
    const float gradient[6];
} chip_theme;

struct tex_coords {
    tex_coords() = default;

    tex_coords(const rect<float> &rgb) {
        uv[0] = uv[1] = uv[2] = rgb;
    }

    tex_coords(const rect<float> &r, const rect<float> &g, const rect<float> &b) {
        uv[0] = r;
        uv[1] = g;
        uv[2] = b;
    }

    rect<float> uv[3];
};

namespace assets {
    static const unsigned short goals_count = 75;
    static const unsigned short goals_for_hearts = 65;
    static const unsigned short default_chips_count = 3;

    extern const long long      ms_for_1s;
    extern const double         deceleration_factor;
    extern const float          seconds_for_60_fps;
    extern const float          info_display_time_s;
    extern const float          cue_show_time_s;
    extern const float          cue_animation_duration_s;
    extern const float          cue_hide_time_s;
    extern const float          touch_hide_time_s;
    extern const float          ui_disabled_alpha;
    extern const float          ui_transition_step;
    extern const float          ui_page_indicator_size;
    extern const float          ui_margin;
    extern const float          ui_paragraph_margin;
    extern const float          ui_goal_label_height;
    extern const float          ui_button_size;
    extern const float          ui_small_alert_height;
    extern const float          ui_large_alert_height;
    extern const float          ui_purchase_row_height;
    extern const float          ui_progress_view_height;
    extern const float          ui_rate_app_view_height;
    extern const float          ui_play_on_view_height;
    extern const float          ui_video_button_size;
    extern const float          ui_empty_space_multiplier;
    extern const float          ui_font_fill_ratio;
    extern const float          ui_progress_line_thickness;
    extern const float          ui_progress_circle_thickness;
    extern const float          ui_background_view_alpha;
    extern const unsigned short ui_purchase_items_count;
    extern const unsigned short max_chip_level;
    extern const unsigned short goals_for_hearts_purchase;
    extern const float          video_view_size;
    extern const vec2<float>    shadow_texture_size;
    extern const vec2<float>    texture_atlas_size;
    extern vec2<float>          screen_size;
    extern chip_theme           chip_themes[chip_type_range];
    extern IStringLoader       *loader;

    namespace glyphs {
        extern const wchar_t circle;
        extern const wchar_t shadow;
        extern const wchar_t blend_spot;
        extern const wchar_t tick;
        extern const wchar_t supercharger;
        extern const wchar_t heart;
        extern const wchar_t half_heart;
        extern const wchar_t twitter;
        extern const wchar_t locked;
        extern const wchar_t buttons[button_type_range];
        extern const wchar_t blank_chip;
        extern const wchar_t chips[chip_type_range];
        extern const wchar_t gradients[chip_type_range];
    }
    extern const std::unordered_map<wchar_t, tex_coords> texture_atlas;
}

#endif
