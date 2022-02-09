// Copyright © 2018 Igor Pener. All rights reserved.

#include "AlertView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_tick,
    label_title,
    label_info,
    label_range
};

AlertView::AlertView(IViewDelegate<AlertView> *delegate) : TView<AlertView>(delegate, label_range) {
    _labels[button_tick] = {
        {screen_size.x / 2.f, (screen_size.y - ui_small_alert_height + ui_button_size) / 2.f + 2.f * ui_margin},
        ui_button_size,
        glyphs::buttons[button_type_tick],
        font_weight::thin
    };
    _labels[label_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        ::fill_type::rounded,
        loader->localizedString("charging_info"),
        alignment_type::left,
        font_weight::thick
    };
}

void AlertView::setTransitionProgress(float progress) {
    _blur_data.x = progress * screen_size.x;

    _labels[button_tick].setPosition({screen_size.x / 2.f + _blur_data.x, _labels[button_tick].p.y});
    _labels[label_title].setPosition({_labels[button_tick].p.x, _labels[label_title].p.y});

    if (progress > -0.99999f && progress < 0.99999f) {
        _blur_data.z = 1.f;
        _labels[button_tick].alpha = 1.f;
        _labels[label_title].alpha = 1.f;
    } else {
        _blur_data.z = 0.f;
        _labels[button_tick].alpha = 0.f;
        _labels[label_title].alpha = 0.f;
    }
    _labels[label_info].alpha = _info_visible ? 1.f - fabs(progress) : 0.f;
}

view_type AlertView::type() const {
    return view_type::small_alert;
}

template<> bool AlertView::tapped<button_type_tick>(const vec2<float> &p) const {
    return _labels[button_tick].tapped(p);
}

void AlertView::setTitle(tutorial_type type) {
    _intro_showing = false;
    setTitle(loader->key(type) + "_a");
}

void AlertView::setTitle(intro_type type, float origin_y) {
    _intro_showing = true;
    setTitle(loader->key(type));

    if (type == intro_type_charging) {
        _info_visible = true;
        _labels[label_info].bounds.size = {
            _labels[label_info].size.x + 3.f * ui_margin, _labels[label_info].size.y + ui_margin
        };
        _labels[label_info].bounds.origin = {
            - (_labels[label_info].bounds.size.x - _labels[label_info].size.x) / 2.f,
            - (_labels[label_info].bounds.size.y - _labels[label_info].size.y) / 2.f
        };
        _labels[label_info].setPosition({
            ui_paragraph_margin + _labels[label_info].bounds.size.x / 2.f,
            origin_y + _labels[label_info].bounds.size.y / 2.f + ui_paragraph_margin
        });
        _labels[label_info].pointer = _labels[label_info].bounds.size.y / _labels[label_info].bounds.size.x;
    } else {
        _info_visible = false;
    }
}

bool AlertView::introShowing() const {
    return _intro_showing;
}

void AlertView::setTitle(const std::string &key) {
    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString(key.c_str()),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        ((screen_size.y + ui_small_alert_height + _labels[button_tick].size.y) / 2.f + _labels[button_tick].p.y) / 2.f
    });
}
