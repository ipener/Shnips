// Copyright © 2018 Igor Pener. All rights reserved.

#include "GoalsView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_video,
    button_cross,
    button_heart,
    label_progress,
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

GoalsView::GoalsView(IViewDelegate<GoalsView> *delegate) : TView<GoalsView>(delegate, label_range, line_range) {
    _labels[button_video] = {
        {ui_paragraph_margin, ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_ad],
        font_weight::thin,
        fill_type::inverted
    };
    _labels[button_video].bounds = {{0.f, 0.f}, {2.f * ui_paragraph_margin, 2.f * ui_paragraph_margin}};
    _labels[button_cross] = {
        {screen_size.x - ui_paragraph_margin, ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin,
        fill_type::inverted
    };
    _labels[button_cross].bounds = {{screen_size.x - 2.f * ui_paragraph_margin, 0.f}, _labels[button_video].bounds.size};
    _labels[button_heart] = {
        {ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_heart],
        font_weight::thin
    };

    _labels[label_button_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::rounded,
        loader->localizedString("heart_info"),
        alignment_type::left,
        font_weight::thick
    };
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

    _lines[line_notification] = {
        {screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (30.f + 6.f) / 2.f - 12.f - 20.f - 6.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
}

void GoalsView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    const auto notification_alpha = progress * _notification_alpha;

    _labels[button_video].alpha = progress;
    _labels[button_cross].alpha = progress;
    _labels[button_heart].alpha = progress * _heart_alpha;

    _labels[label_progress].alpha = progress;
    _labels[label_notification].alpha = notification_alpha;
    _labels[label_notification_info].alpha = notification_alpha;
    _labels[label_info].alpha = progress;
    _labels[label_button_info].alpha = progress * _button_info_alpha;

    _lines[line_notification].alpha = notification_alpha;
}

template<> bool GoalsView::tapped<button_type_ad>(const vec2<float> &p) const {
    return _labels[button_video].tapped(p) || (_labels[button_video].text.size() && _labels[label_info].tapped(p));
}

template<> bool GoalsView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

template<> bool GoalsView::tapped<button_type_heart>(const vec2<float> &p) const {
    return _labels[button_heart].tapped(p);
}

void GoalsView::setProgress(unsigned long long progress, unsigned long long count) {
    _labels[label_progress].setText(
        30.f,
        screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
        loader->localizedString("progress", progress, count),
        alignment_type::center
    );
    _labels[label_progress].setPosition({
        screen_size.x / 2.f, screen_size.y - ui_paragraph_margin
    });
}

void GoalsView::setNotification(skill_shot_type type) {
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

void GoalsView::setNotificationAlpha(float alpha) {
    _notification_alpha = alpha;
    _labels[label_notification].alpha = _notification_alpha;
    _labels[label_notification_info].alpha = _notification_alpha;
    _lines[line_notification].alpha = _notification_alpha;
}

void GoalsView::setButtonInfoAlpha(float alpha) {
    _button_info_alpha = alpha;
    _labels[label_button_info].alpha = _button_info_alpha;
}

void GoalsView::setGoal(unsigned short goal) {
    const std::string key("goal_" + std::to_string(goal));
    _labels[label_info] = {
        14.f,
        screen_size.x - 4.f * ui_paragraph_margin,
        fill_type::inverted,
        loader->localizedString(key.c_str()),
        alignment_type::center,
        font_weight::regular
    };
    _labels[label_info].bounds = {
        {0.f, - (_labels[button_video].bounds.size.y - _labels[label_info].size.y) / 2.f},
        {screen_size.x - 2.f * _labels[button_video].bounds.size.x, _labels[button_video].bounds.size.y}
    };
    _labels[label_info].setPosition({screen_size.x / 2.f, ui_paragraph_margin});
}

void GoalsView::setVideoButtonEnabled(bool enabled) {
    if (enabled) {
        _labels[button_video].setText(
            {ui_paragraph_margin, ui_paragraph_margin},
            ui_button_size,
            glyphs::buttons[button_type_ad],
            font_weight::thin
        );
    } else {
        _labels[button_video].clearText();
    }
}

void GoalsView::setHeartButtonEnabled(bool enabled) {
    _heart_alpha = enabled ? 1.f : 0.f;
}
