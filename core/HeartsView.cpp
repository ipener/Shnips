// Copyright © 2018 Igor Pener. All rights reserved.

#include "HeartsView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_cross,
    label_title,
    label_heart_0,
    label_heart_1,
    label_heart_2,
    label_price,
    label_range
};

HeartsView::HeartsView(IViewDelegate<HeartsView> *delegate) : TView<HeartsView>(delegate, label_range) {
    _labels[button_cross] = {
        {screen_size.x / 2.f, (screen_size.y - ui_large_alert_height + ui_button_size) / 2.f + 2.f * ui_margin},
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin
    };
    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("heart_info"),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_large_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_heart_0] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        100.f,
        glyphs::heart,
        font_weight::thin
    };
    _labels[label_heart_1] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        100.f,
        glyphs::half_heart,
        font_weight::thin
    };
    _labels[label_heart_2] = {
        {screen_size.x / 2.f, screen_size.y / 2.f + (20.f + 2.f * ui_margin) / 2.f},
        100.f,
        glyphs::half_heart,
        font_weight::thin
    };

    setHeartPrice(loader->localizedString("get"));
}

void HeartsView::setTransitionProgress(float progress) {
    _blur_data.x = progress * screen_size.x;

    _labels[button_cross].setPosition({screen_size.x / 2.f + _blur_data.x, _labels[button_cross].p.y});
    _labels[label_title].setPosition({_labels[button_cross].p.x, _labels[label_title].p.y});

    const auto s = 0.525f;
    const auto w = _labels[label_heart_0].size.x + _labels[label_heart_1].size.x * s + _labels[label_heart_2].size.x * s;

    _labels[label_heart_0].setPosition({
        (screen_size.x - w + _labels[label_heart_0].size.x) / 2.f + _blur_data.x, _labels[label_heart_0].p.y
    });
    _labels[label_heart_1].setPosition({
        _labels[label_heart_0].p.x + (_labels[label_heart_0].size.x + _labels[label_heart_1].size.x * (2.f * s - 1.f)) / 2.f, _labels[label_heart_1].p.y
    });
    _labels[label_heart_2].setPosition({
        _labels[label_heart_1].p.x + (_labels[label_heart_1].size.x + _labels[label_heart_2].size.x * (2.f * s - 1.f)) / 2.f, _labels[label_heart_2].p.y
    });
    _labels[label_price].setPosition({
        _labels[button_cross].p.x, _labels[label_heart_0].p.y - _labels[label_heart_0].size.y / 2.f - ui_margin
    });

    if (progress > -0.99999f && progress < 0.99999f) {
        _blur_data.z = 1.f;
        _labels[button_cross].alpha = 1.f;
        _labels[label_title].alpha = 1.f;
        _labels[label_heart_0].alpha = 1.f;
        _labels[label_heart_1].alpha = 1.f;
        _labels[label_heart_2].alpha = 1.f;
        _labels[label_price].alpha = 1.f;
    } else {
        _blur_data.z = 0.f;
        _labels[button_cross].alpha = 0.f;
        _labels[label_title].alpha = 0.f;
        _labels[label_heart_0].alpha = 0.f;
        _labels[label_heart_1].alpha = 0.f;
        _labels[label_heart_2].alpha = 0.f;
        _labels[label_price].alpha = 0.f;
    }
}

view_type HeartsView::type() const {
    return view_type::large_alert;
}

template<> bool HeartsView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

bool HeartsView::buyButtonTapped(const vec2<float> &p) const {
    return _labels[label_price].tapped(p);
}

void HeartsView::setHeartPrice(const std::wstring &price) {
    _labels[label_price] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::rounded,
        price,
        alignment_type::left,
        font_weight::thick
    };
    _labels[label_price].bounds.size = {
        _labels[label_price].size.x + 3.f * ui_margin, _labels[label_price].size.y + ui_margin
    };
    _labels[label_price].bounds.origin = {
        - (_labels[label_price].bounds.size.x - _labels[label_price].size.x) / 2.f,
        - (_labels[label_price].bounds.size.y - _labels[label_price].size.y) / 2.f
    };
    _labels[label_price].setPosition({
        screen_size.x - ui_paragraph_margin - _labels[label_price].bounds.size.x / 2.f,
        _labels[label_heart_0].p.y + _labels[label_heart_0].size.y + ui_margin
    });
}
