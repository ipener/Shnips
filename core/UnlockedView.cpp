// Copyright © 2018 Igor Pener. All rights reserved.

#include "UnlockedView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_store,
    button_tick,
    label_title,
    label_unlockable,
    label_heart_0,
    label_heart_1,
    label_heart_2,
    label_range
};

UnlockedView::UnlockedView(IViewDelegate<UnlockedView> *delegate) : TView<UnlockedView>(delegate, label_range) {
    _labels[button_store] = {
        {screen_size.x / 2.f, (screen_size.y - ui_small_alert_height + ui_button_size) / 2.f + 2.f * ui_margin},
        ui_button_size,
        glyphs::buttons[button_type_store],
        font_weight::thin
    };
    _labels[button_tick] = {
        _labels[button_store].p,
        ui_button_size,
        glyphs::buttons[button_type_tick],
        font_weight::thin
    };

    _labels[label_heart_0] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        100.f,
        glyphs::heart,
        font_weight::thin
    };
    _labels[label_heart_1] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        100.f,
        glyphs::half_heart,
        font_weight::thin
    };
    _labels[label_heart_2] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        100.f,
        glyphs::half_heart,
        font_weight::thin
    };
}

void UnlockedView::setTransitionProgress(float progress) {
    _blur_data.x = progress * screen_size.x;

    _labels[button_store].setPosition({screen_size.x / 2.f + _blur_data.x, _labels[button_store].p.y});
    _labels[button_tick].setPosition({_labels[button_store].p.x, _labels[button_tick].p.y});
    _labels[label_title].setPosition({_labels[button_store].p.x, _labels[label_title].p.y});
    _labels[label_unlockable].setPosition({_labels[button_store].p.x, _labels[label_unlockable].p.y});

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

    if (progress > -0.99999f && progress < 0.99999f) {
        _blur_data.z = 1.f;
        _labels[_show_store_button ? button_store : button_tick].alpha = 1.f;
        _labels[label_title].alpha = 1.f;
        if (_show_hearts) {
            _labels[label_heart_0].alpha = 1.f;
            _labels[label_heart_1].alpha = 1.f;
            _labels[label_heart_2].alpha = 1.f;
        } else {
            _labels[label_unlockable].alpha = 1.f;
        }
    } else {
        _blur_data.z = 0.f;
        _labels[button_store].alpha = 0.f;
        _labels[button_tick].alpha = 0.f;
        _labels[label_title].alpha = 0.f;
        _labels[label_unlockable].alpha = 0.f;
        _labels[label_heart_0].alpha = 0.f;
        _labels[label_heart_1].alpha = 0.f;
        _labels[label_heart_2].alpha = 0.f;
    }
}

view_type UnlockedView::type() const {
    return view_type::small_alert;
}

template<> bool UnlockedView::tapped<button_type_store>(const vec2<float> &p) const {
    return _labels[button_store].tapped(p);
}

template<> bool UnlockedView::tapped<button_type_tick>(const vec2<float> &p) const {
    return _labels[button_tick].tapped(p);
}

void UnlockedView::setChip(chip_type type) {
    _show_store_button = true;
    _show_hearts = false;
    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("unlocked", loader->localizedString(loader->key(type).c_str()).c_str()),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_small_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_unlockable] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        50.f,
        glyphs::chips[type],
        static_cast<font_weight>(0)
    };
}

void UnlockedView::setSuperchargerUnlocked() {
    _show_store_button = false;
    _show_hearts = false;
    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("unlocked", loader->localizedString("supercharger").c_str()),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_small_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_unlockable] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        50.f,
        glyphs::supercharger,
        static_cast<font_weight>(0)
    };
}

void UnlockedView::setHeartsUnlocked() {
    _show_store_button = false;
    _show_hearts = true;
    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("unlocked", loader->localizedString("three_lives").c_str()),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_small_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_unlockable] = {
        {screen_size.x / 2.f, screen_size.y / 2.f},
        50.f,
        glyphs::supercharger,
        static_cast<font_weight>(0)
    };
}
