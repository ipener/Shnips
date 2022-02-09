// Copyright © 2018 Igor Pener. All rights reserved.

#include "TutorialView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_cross,
    label_score,
    label_shot_score,
    label_notification,
    label_notification_info,
    label_range
};

enum {
    line_notification,
    line_range
};

TutorialView::TutorialView(IViewDelegate<TutorialView> *delegate) : TView<TutorialView>(delegate, label_range, line_range) {
    _labels[button_cross] = {
        {screen_size.x - ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin
    };
    
    _lines[line_notification] = {
        {screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (30.f + 6.f) / 2.f - 12.f - 20.f - 6.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
}

void TutorialView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);

    const auto notification_alpha = progress * _notification_alpha;
    _labels[button_cross].alpha = _cancelable ? progress : 0.f;
    _labels[label_score].alpha = progress;
    _labels[label_shot_score].alpha = progress;
    _labels[label_notification].alpha = notification_alpha;
    _labels[label_notification_info].alpha = notification_alpha;
    _lines[line_notification].alpha = notification_alpha;
}

template<> bool TutorialView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

void TutorialView::setScore(unsigned long long score) {
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

void TutorialView::setShotScore(unsigned long long score) {
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

void TutorialView::setNotification(tutorial_type type) {
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

void TutorialView::setNotificationAlpha(float alpha) {
    _notification_alpha = alpha;
    _labels[label_notification].alpha = _notification_alpha;
    _labels[label_notification_info].alpha = _notification_alpha;
    _lines[line_notification].alpha = _notification_alpha;
}

void TutorialView::setCancelable(bool cancelable) {
    _cancelable = cancelable;
}
