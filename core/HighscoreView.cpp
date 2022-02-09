// Copyright © 2018 Igor Pener. All rights reserved.

#include "HighscoreView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_menu,
    button_supercharger,
    button_heart,
    label_score,
    label_shot_score,
    label_notification,
    label_notification_info,
    label_info,
    label_button_info,
    label_range
};

enum {
    line_notification,
    line_range
};

HighscoreView::HighscoreView(IViewDelegate<HighscoreView> *delegate) : TView<HighscoreView>(delegate, label_range, line_range) {
    _labels[button_menu] = {
        {screen_size.x - ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_menu],
        font_weight::thin
    };
    _labels[button_supercharger] = {
        {ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_supercharger],
        font_weight::thin
    };
    _labels[button_heart] = {
        {ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_heart],
        font_weight::thin
    };
    _labels[label_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::rounded,
        loader->localizedString("blank_chip_info"),
        alignment_type::left,
        font_weight::thick
    };
    _labels[label_info].bounds.size = {
        _labels[label_info].size.x + 3.f * ui_margin, _labels[label_info].size.y + ui_margin
    };
    _labels[label_info].bounds.origin = {
        - (_labels[label_info].bounds.size.x - _labels[label_info].size.x) / 2.f,
        - (_labels[label_info].bounds.size.y - _labels[label_info].size.y) / 2.f
    };
    _labels[label_info].setPosition({screen_size.x / 2.f, screen_size.y / 4.f});

    _lines[line_notification] = {
        {screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (30.f + 6.f) / 2.f - 12.f - 20.f - 6.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
}

void HighscoreView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    const auto notification_alpha = progress * _notification_alpha;

    _labels[button_menu].alpha = progress;
    _labels[button_supercharger].alpha = progress * _supercharger_alpha;
    _labels[button_heart].alpha = progress * _heart_alpha;

    _labels[label_score].alpha = progress;
    _labels[label_shot_score].alpha = progress;
    _labels[label_notification].alpha = notification_alpha;
    _labels[label_notification_info].alpha = notification_alpha;
    _labels[label_info].alpha = progress * _bar_info_alpha;
    _labels[label_button_info].alpha = progress * _button_info_alpha;

    _lines[line_notification].alpha = notification_alpha;
}

template<> bool HighscoreView::tapped<button_type_menu>(const vec2<float> &p) const {
    return _labels[button_menu].tapped(p);
}

template<> bool HighscoreView::tapped<button_type_supercharger>(const vec2<float> &p) const {
    return _labels[button_supercharger].tapped(p);
}

template<> bool HighscoreView::tapped<button_type_heart>(const vec2<float> &p) const {
    return _labels[button_heart].tapped(p);
}

void HighscoreView::setScore(unsigned long long score) {
    _labels[label_score].setText(
        30.f,
        screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
        std::to_wstring(score),
        alignment_type::center
    );
    _labels[label_score].setPosition({
        screen_size.x / 2.f, screen_size.y - ui_paragraph_margin
    });
    _labels[label_shot_score].clearText();
}

void HighscoreView::setShotScore(unsigned long long score) {
    if (score) {
        _labels[label_shot_score].setText(
            12.f,
            screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
            L"+" + std::to_wstring(score),
            alignment_type::center,
            font_weight::thick
        );
        _labels[label_shot_score].setPosition({
            screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (12.f + 30.f) / 2.f
        });
    } else {
        _labels[label_shot_score].clearText();
    }
}

void HighscoreView::setNotification(skill_shot_type type) {
    auto key = loader->key(type);
    _labels[label_notification] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString(key.c_str()),
        alignment_type::center
    };
    _labels[label_notification].setPosition({
        screen_size.x / 2.f,
        screen_size.y - ui_paragraph_margin - 12.f - (_labels[label_notification].size.y + 30.f) / 2.f - 6.f
    });
    key += "_d";
    _labels[label_notification_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString(key.c_str()),
        alignment_type::center,
        font_weight::thick
    };
    _labels[label_notification_info].setPosition({
        screen_size.x / 2.f,
        _labels[label_notification].p.y - (_labels[label_notification].size.y + _labels[label_notification_info].size.y) / 2.f - 6.f
    });
}

void HighscoreView::setNotificationAlpha(float alpha) {
    _notification_alpha = alpha;
    _labels[label_notification].alpha = _notification_alpha;
    _labels[label_notification_info].alpha = _notification_alpha;
    _lines[line_notification].alpha = _notification_alpha;
}

void HighscoreView::setBarInfoAlpha(float alpha) {
    _bar_info_alpha = alpha;
    _labels[label_info].alpha = _bar_info_alpha;
}

void HighscoreView::setButtonInfoAlpha(float alpha) {
    _button_info_alpha = alpha;
    _labels[label_button_info].alpha = _button_info_alpha;
}

void HighscoreView::setSuperchargerAlpha(float alpha) {
    _supercharger_alpha = alpha;
    _labels[button_supercharger].alpha = _supercharger_alpha;
}

void HighscoreView::setHeartButtonEnabled(bool enabled) {
    if (enabled) {
        _heart_alpha = 1.f;
        _labels[label_button_info] = {
            12.f,
            screen_size.x - 2.f * ui_paragraph_margin,
            fill_type::rounded,
            loader->localizedString("heart_info"),
            alignment_type::left,
            font_weight::thick
        };
    } else {
        _heart_alpha = 0.f;
        _labels[label_button_info] = {
            12.f,
            screen_size.x - 2.f * ui_paragraph_margin,
            fill_type::rounded,
            loader->localizedString("supercharger_info"),
            alignment_type::left,
            font_weight::thick
        };
    }

    _labels[label_button_info].bounds.size = {
        _labels[label_button_info].size.x + 3.f * ui_margin,
        _labels[label_button_info].size.y + ui_margin
    };
    _labels[label_button_info].bounds.origin = {
        - (_labels[label_button_info].bounds.size.x - _labels[label_button_info].size.x) / 2.f,
        - (_labels[label_button_info].bounds.size.y - _labels[label_button_info].size.y) / 2.f
    };
    _labels[label_button_info].setPosition({
        ui_margin + _labels[label_button_info].bounds.size.x / 2.f,
        _labels[button_heart].p.y - (_labels[button_heart].size.y + _labels[label_button_info].bounds.size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_button_info].pointer = -_labels[label_button_info].bounds.size.y / _labels[label_button_info].bounds.size.x;
}

