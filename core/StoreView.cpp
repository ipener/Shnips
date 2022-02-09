// Copyright © 2018 Igor Pener. All rights reserved.

#include "StoreView.hpp"

#include "assets.hpp"
#include "Chip.hpp"

using namespace assets;

enum {
    button_tick,
    button_cross,
    label_title,
    label_ability_0,
    label_ability_1,
    label_ability_2,
    label_indicator_0,
    label_indicator_1,
    label_indicator_2,
    label_indicator_3,
    label_indicator_4,
    label_unlockable,
    label_info,
    label_range
};

enum {
    line_ability_1,
    line_ability_2,
    line_range
};

enum {
    circle_progress,
    circle_range
};

StoreView::StoreView(IViewDelegate<StoreView> *delegate) : TView<StoreView>(delegate, label_range, line_range, circle_range) {
    const rect<double> bounds {
        {}, {static_cast<double>(screen_size.x), static_cast<double>(screen_size.y)}
    };
    vec2<double> center {1e100, -1e100};
    Chip::defaultChipPositions(default_chips_count, bounds, [&center](const vec2<double> &p) {
        center.x = std::min(center.x, p.x);
        center.y = std::max(center.y, p.y);
    });
    _zoomed_in_data.x = static_cast<float>(center.x);
    _zoomed_in_data.y = static_cast<float>(center.y + Chip::default_size * 2.0);
    _zoomed_in_data.z = static_cast<float>(Chip::default_size * 4.0);

    _labels[button_tick] = {
        {screen_size.x / 2.f, ui_button_size / 2.f + ui_margin},
        ui_button_size,
        glyphs::buttons[button_type_tick],
        font_weight::thin
    };
    _labels[button_cross] = {
        _labels[button_tick].p,
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin
    };
    vec2<float> p {
        screen_size.x / 2.f - (chip_type_range - 1) * ui_page_indicator_size,
        screen_size.y - ui_margin - ui_page_indicator_size / 2.f
    };
    for (short i = 0; i < chip_type_range; ++i) {
        _labels[label_indicator_0 + i] = {
            p,
            ui_page_indicator_size,
            glyphs::locked,
            font_weight::thin
        };
        p.x += 2.f * ui_page_indicator_size;
    }

    _lines[line_ability_1] = {
        {screen_size.x / 2.f, screen_size.y * 0.75f - ui_button_size / 2.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
    _lines[line_ability_2] = {
        {_lines[line_ability_1].p.x, _lines[line_ability_1].p.y - ui_button_size},
        _lines[line_ability_1].size
    };

    _circles[circle_progress] = {screen_size / 2.f, ui_button_size};
}

void StoreView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);

    _zoom_data.x = screen_size.x / 2.f * (1.f - progress) + _zoomed_in_data.x * progress;
    _zoom_data.y = screen_size.y / 2.f * (1.f - progress) + _zoomed_in_data.y * progress;
    _zoom_data.z = screen_size.y / 2.f * (1.f - progress) + _zoomed_in_data.z * progress;

    _labels[label_indicator_0 + _chip_type].alpha = progress;
    for (short i = 1; i < chip_type_range; ++i) {
        _labels[label_indicator_0 + (_chip_type + i) % chip_type_range].alpha = progress * ui_disabled_alpha;
    }
    setViewAlpha(progress);
}

template<> bool StoreView::tapped<button_type_tick>(const vec2<float> &p) const {
    return _labels[button_tick].tapped(p);
}

template<> bool StoreView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

void StoreView::setViewAlpha(float alpha) {
    _labels[label_title].alpha = alpha;
    _labels[label_ability_0].alpha = alpha;
    _labels[label_unlockable].alpha = alpha * ui_disabled_alpha;
    _lines[line_ability_1].alpha = alpha;

    if (_chip_selectable) {
        _labels[button_tick].alpha = alpha;
        _labels[button_cross].alpha = 0.f;
        _labels[label_ability_1].alpha = _chip_progress > 0.99999f ? alpha : ui_disabled_alpha;
        _labels[label_ability_2].alpha = _chip_progress > 1.99999f ? alpha : ui_disabled_alpha;
        _labels[label_info].alpha = 0.f;

        if (_chip_type == chip_type_default) {
            _lines[line_ability_2].alpha = 0.f;
        } else {
            _lines[line_ability_2].alpha = _chip_progress > 0.99999f ? alpha : ui_disabled_alpha;
        }
    } else {
        _labels[button_tick].alpha = 0.f;
        _labels[button_cross].alpha = alpha;
        _labels[label_ability_1].alpha = 0.f;
        _labels[label_ability_2].alpha = 0.f;
        _labels[label_info].alpha = alpha;
        _lines[line_ability_2].alpha = 0.f;
    }
    if (_chip_progress < 1.99999f) {
        _circles[circle_progress].color.a = alpha;
    } else {
        _circles[circle_progress].color.a = 0.f;
    }
}

void StoreView::setChipType(chip_type type, unsigned short level, float progress) {
    _chip_type = type;
    _chip_progress = level > 1 ? 1.f + progress : progress;
    _chip_selectable = level > 0;
    _circles[circle_progress].progress = progress;

    auto key = loader->key(type);
    const float h = 70.f;
    _labels[label_title].setText(
        h,
        screen_size.x - 2.f * ui_paragraph_margin,
        loader->localizedString(key.c_str()),
        alignment_type::center,
        font_weight::thin
    );

    key += "_0";
    if (_chip_selectable) {
        if (type == chip_type_default) {
            _labels[label_ability_0].setText(
                16.f,
                screen_size.x - 2.f * ui_paragraph_margin,
                loader->localizedString(key.c_str()),
                alignment_type::center,
                font_weight::regular
            );
            _labels[label_ability_0].setPosition({screen_size.x / 2.f, _lines[line_ability_1].p.y + 16.f});

            _labels[label_ability_1].clearText();
            _labels[label_ability_2].clearText();
        } else {
            _labels[label_ability_0].setText(
                16.f,
                screen_size.x - 2.f * ui_paragraph_margin,
                loader->localizedString("chip_ability_0", loader->localizedString(key.c_str())),
                alignment_type::left,
                font_weight::regular
            );
            _labels[label_ability_0].setPosition({
                ui_paragraph_margin + _labels[label_ability_0].size.x / 2.f, _lines[line_ability_1].p.y + 16.f
            });

            key[key.length() - 1] = '1';
            _labels[label_ability_1].setText(
                16.f,
                screen_size.x - 2.f * ui_paragraph_margin,
                loader->localizedString("chip_ability_1", loader->localizedString(key.c_str())),
                alignment_type::left,
                font_weight::regular
            );
            _labels[label_ability_1].setPosition({
                ui_paragraph_margin + _labels[label_ability_1].size.x / 2.f, _lines[line_ability_2].p.y + 16.f
            });
            key[key.length() - 1] = '2';
            _labels[label_ability_2].setText(
                16.f,
                screen_size.x - 2.f * ui_paragraph_margin,
                loader->localizedString("chip_ability_2", loader->localizedString(key.c_str())),
                alignment_type::left,
                font_weight::regular
            );
            _labels[label_ability_2].setPosition({
                ui_paragraph_margin + _labels[label_ability_2].size.x / 2.f, _lines[line_ability_2].p.y - 16.f
            });
        }
        if (level < 3) {
            _labels[label_unlockable] = {
                {screen_size.x / 2.f, screen_size.y / 2.f},
                50.f,
                glyphs::chips[type],
                static_cast<font_weight>(level)
            };
        } else {
            _labels[label_unlockable].clearText();
        }
    } else {
        _labels[label_ability_0].setText(
            16.f,
            screen_size.x - 2.f * ui_paragraph_margin,
            loader->localizedString(key.c_str()),
            alignment_type::center,
            font_weight::regular
        );
        _labels[label_ability_0].setPosition({screen_size.x / 2.f, _lines[line_ability_1].p.y + 16.f});

        _labels[label_ability_1].clearText();
        _labels[label_ability_2].clearText();

        if (type != chip_type_default) {
            _labels[label_unlockable] = {
                {screen_size.x / 2.f, screen_size.y / 2.f},
                50.f,
                glyphs::locked,
                font_weight::thin
            };
        } else {
            _labels[label_unlockable].clearText();
        }
    }
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (_labels[label_indicator_0].p.y + _labels[label_ability_0].p.y) / 2.f - (ui_page_indicator_size - _labels[label_ability_0].size.y) / 4.f
    });
}

void StoreView::setGoalsProgress(unsigned short completed, unsigned short total) {
    _labels[label_info].setText(
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        loader->localizedString("goals_progress", completed, total),
        alignment_type::center,
        font_weight::regular
    );
    _labels[label_info].setPosition({screen_size.x / 2.f, screen_size.y / 4.f});
}

void StoreView::setIndicatorLevel(unsigned short index, unsigned short level) {
    if (index == chip_type_range)
        return;

    const auto p = _labels[label_indicator_0 + index].p;
    if (level > 0) {
        _labels[label_indicator_0 + index].setText(
            p,
            ui_page_indicator_size,
            glyphs::chips[index],
            static_cast<font_weight>(index == chip_type_default ? 0 : level - 1)
        );
    } else {
        _labels[label_indicator_0 + index].setText(
            p,
            ui_page_indicator_size,
            glyphs::locked,
            font_weight::thin
        );
    }
}

void StoreView::setIndicatorAlpha(unsigned short index, float alpha) {
    if (index == chip_type_range)
        return;

    _labels[label_indicator_0 + index].alpha = alpha;
}

void StoreView::setBlurIntensity(float blur_intensity) {
    _blur_data.z = blur_intensity;
}
