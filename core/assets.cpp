// Copyright © 2018 Igor Pener. All rights reserved.

#include "assets.hpp"

namespace assets {
    const long long ms_for_1s = 1000;
    const double deceleration_factor = 0.3;
    const float seconds_for_60_fps = 0.0166667f;
    const float info_display_time_s = 3.f;
    const float cue_show_time_s = 0.3f;
    const float cue_animation_duration_s = 2.f;
    const float cue_hide_time_s = 1.f;
    const float touch_hide_time_s = 0.8f;
    const float ui_disabled_alpha = 0.15f;
    const float ui_transition_step = 2.f;
    const float ui_page_indicator_size = 24.f;
    const float ui_margin = 8.f;
    const float ui_paragraph_margin = ui_margin + 16.f;
    const float ui_goal_label_height = 2.f * ui_paragraph_margin;
    const float ui_button_size = 32.f;
    const float ui_small_alert_height = 200.f;
    const float ui_large_alert_height = 286.f;
    const float ui_purchase_row_height = 52.f;
    const float ui_progress_view_height = 176.f;
    const float ui_rate_app_view_height = 96.f;
    const float ui_play_on_view_height = 160.f;
    const float ui_video_button_size = 13.f;
    const float ui_empty_space_multiplier = 0.3f;
    const float ui_font_fill_ratio = 0.6f;
    const float ui_progress_line_thickness = 5.f;
    const float ui_progress_circle_thickness = 2.f;
    const float ui_background_view_alpha = 0.2f;
    const unsigned short ui_purchase_items_count = 6;
    const unsigned short max_chip_level = 3;
    const unsigned short goals_for_hearts_purchase = 5;
    const float video_view_size = 400.f;
    const vec2<float> shadow_texture_size {256.f, 168.f};
    const vec2<float> texture_atlas_size {2048.f, 2048.f};
    vec2<float> screen_size;
    IStringLoader *loader = nullptr;

    namespace glyphs {
        const wchar_t circle = L'\ue900';
        const wchar_t shadow = L'\ue901';
        const wchar_t blend_spot = L'\ue902';
        const wchar_t tick = L'\ue903';
        const wchar_t supercharger = L'\ue904';
        const wchar_t heart = L'\ue905';
        const wchar_t half_heart = L'\ue906';
        const wchar_t twitter = L'\ue907';
        const wchar_t locked = L'\ue908';
        const wchar_t buttons[button_type_range] {
            L'\ue910',
            L'\ue911',
            L'\ue912',
            L'\ue913',
            L'\ue914',
            L'\ue915',
            L'\ue916',
            L'\ue917',
            L'\ue918',
            L'\ue919',
            L'\ue91a',
            L'\ue91b',
            L'\ue91c',
            L'\ue91d'
        };
        const wchar_t blank_chip = L'\ue920';
        const wchar_t chips[chip_type_range] {
            L'\ue921',
            L'\ue922',
            L'\ue923',
            L'\ue924',
            L'\ue925'
        };
        const wchar_t gradients[chip_type_range] {
            L'\ue926',
            L'\ue927',
            L'\ue928',
            L'\ue929',
            L'\ue92a'
        };
    }

    chip_theme chip_themes[chip_type_range] {
        {
            0.f,
            {
                255 / 255.f, 158 / 255.f, 119 / 255.f,
                255 / 255.f,  23 / 255.f, 90 / 255.f
            }
        },
        {
            0.f,
            {
                255 / 255.f, 225 / 255.f, 116 / 255.f,
                255 / 255.f, 155 / 255.f,  79 / 255.f
            }
        },
        {
            0.f,
            {
                255 / 255.f, 122 / 255.f, 192 / 255.f,
                142 / 255.f,  76 / 255.f, 212 / 255.f
            }
        },
        {
            0.f,
            {
                130 / 255.f, 230 / 255.f, 255 / 255.f,
                 50 / 255.f, 160 / 255.f, 255 / 255.f
            }
        },
        {
            0.f,
            {
                169 / 255.f, 255 / 255.f, 195 / 255.f,
                 72 / 255.f, 255 / 255.f, 234 / 255.f
            }
        }
    };
}
