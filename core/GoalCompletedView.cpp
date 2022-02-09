// Copyright © 2018 Igor Pener. All rights reserved.

#include "GoalCompletedView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_tick,
    label_title,
    label_progress,
    label_range
};

GoalCompletedView::GoalCompletedView(IViewDelegate<GoalCompletedView> *delegate) : TView<GoalCompletedView>(delegate, label_range) {
    _labels[button_tick] = {
        {screen_size.x / 2.f, (screen_size.y - ui_small_alert_height + ui_button_size) / 2.f + 2.f * ui_margin},
        ui_button_size,
        glyphs::buttons[button_type_tick],
        font_weight::thin
    };
}

void GoalCompletedView::setTransitionProgress(float progress) {
    _blur_data.x = progress * screen_size.x;

    _labels[button_tick].setPosition({screen_size.x / 2.f + _blur_data.x, _labels[button_tick].p.y});
    _labels[label_title].setPosition({_labels[button_tick].p.x, _labels[label_title].p.y});
    _labels[label_progress].setPosition({_labels[button_tick].p.x, _labels[label_progress].p.y});

    if (progress > -0.99999f && progress < 0.99999f) {
        _blur_data.z = 1.f;
        _labels[button_tick].alpha = 1.f;
        _labels[label_title].alpha = 1.f;
        _labels[label_progress].alpha = 1.f;
    } else {
        _blur_data.z = 0.f;
        _labels[button_tick].alpha = 0.f;
        _labels[label_title].alpha = 0.f;
        _labels[label_progress].alpha = 0.f;
    }
}

view_type GoalCompletedView::type() const {
    return view_type::small_alert;
}

template<> bool GoalCompletedView::tapped<button_type_tick>(const vec2<float> &p) const {
    return _labels[button_tick].tapped(p);
}

void GoalCompletedView::setProgress(unsigned short progress, unsigned short total) {
    _labels[label_progress] = {
        60.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("progress", progress, total),
        alignment_type::center,
        font_weight::thin
    };
    _labels[label_progress].setPosition(screen_size / 2.f);

    _labels[label_title] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("goal_completed"),
        alignment_type::center
    };
    _labels[label_title].setPosition({
        screen_size.x / 2.f,
        (screen_size.y + ui_small_alert_height - _labels[label_title].size.y) / 2.f - 2.f * ui_margin
    });
}
