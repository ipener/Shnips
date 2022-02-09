// Copyright © 2018 Igor Pener. All rights reserved.

#include "TwitterView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_cross,
    label_title,
    label_icon,
    label_range
};

TwitterView::TwitterView(IViewDelegate<TwitterView> *delegate) : TView<TwitterView>(delegate, label_range) {
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
        loader->localizedString("twitter"),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_large_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_icon] = {
        screen_size / 2.f,
        120.f,
        glyphs::twitter,
        font_weight::thin
    };
}

void TwitterView::setTransitionProgress(float progress) {
    _blur_data.x = progress * screen_size.x;

    _labels[button_cross].setPosition({screen_size.x / 2.f + _blur_data.x, _labels[button_cross].p.y});
    _labels[label_title].setPosition({_labels[button_cross].p.x, _labels[label_title].p.y});
    _labels[label_icon].setPosition({_labels[button_cross].p.x, _labels[label_icon].p.y});

    if (progress > -0.99999f && progress < 0.99999f) {
        _blur_data.z = 1.f;
        _labels[button_cross].alpha = 1.f;
        _labels[label_title].alpha = 1.f;
        _labels[label_icon].alpha = 1.f;
    } else {
        _blur_data.z = 0.f;
        _labels[button_cross].alpha = 0.f;
        _labels[label_title].alpha = 0.f;
        _labels[label_icon].alpha = 0.f;
    }
}

view_type TwitterView::type() const {
    return view_type::large_alert;
}

template<> bool TwitterView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

bool TwitterView::iconTapped(const vec2<float> &p) const {
    return _labels[label_icon].tapped(p);
}
