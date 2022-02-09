// Copyright © 2018 Igor Pener. All rights reserved.

#include "HomeView.hpp"

using namespace assets;

enum {
    button_store,
    button_menu,
    button_goals,
    label_highscore,
    label_highscore_info,
    label_goals,
    label_goals_info,
    label_info,
    label_range
};

HomeView::HomeView(IViewDelegate<HomeView> *delegate) : TView<HomeView>(delegate, label_range) {
    _labels[button_store] = {
        {ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_store],
        font_weight::thin
    };
    _labels[button_menu] = {
        {screen_size.x - ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_menu],
        font_weight::thin
    };
    _labels[button_goals] = {
        {screen_size.x - ui_paragraph_margin, ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_goals],
        font_weight::thin
    };
    _labels[label_highscore_info] = {
        12.f,
        screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
        ::fill_type::regular,
        loader->localizedString("highscore"),
        alignment_type::center,
        font_weight::thick
    };
    _labels[label_highscore_info].setPosition({
        screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (12.f + 30.f) / 2.f
    });
    _labels[label_goals_info] = {
        12.f,
        screen_size.x - ui_button_size - 2.f * ui_margin,
        ::fill_type::regular,
        loader->localizedString("goals"),
        alignment_type::right,
        font_weight::thick
    };
    _labels[label_goals_info].setPosition({
        (screen_size.x - ui_button_size - 2.f * ui_margin) / 2.f, ui_margin + 12.f / 2.f
    });
    _labels[label_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        ::fill_type::rounded,
        loader->localizedString("goals_info"),
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
    _labels[label_info].setPosition({
        screen_size.x - ui_margin - _labels[label_info].bounds.size.x / 2.f,
        _labels[button_goals].p.y + (_labels[button_goals].size.y + _labels[label_info].bounds.size.y) / 2.f + 2.f * ui_margin
    });
    _labels[label_info].pointer = 1.f - _labels[label_info].bounds.size.y / _labels[label_info].bounds.size.x;
}

void HomeView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    const float goals_alpha = progress * _goals_alpha;

    _labels[button_store].alpha = progress;
    _labels[button_menu].alpha = progress;
    _labels[button_goals].alpha = goals_alpha;
    
    _labels[label_highscore].alpha = progress * _highscore_alpha;
    _labels[label_highscore_info].alpha = progress * _highscore_alpha;
    _labels[label_goals].alpha = goals_alpha;
    _labels[label_goals_info].alpha = goals_alpha;

    if (_info_showing) {
        _labels[label_info].alpha = goals_alpha;
    } else {
        _labels[label_info].alpha = 0.f;
    }
}

template<> bool HomeView::tapped<button_type_store>(const vec2<float> &p) const {
    return _labels[button_store].tapped(p);
}

template<> bool HomeView::tapped<button_type_menu>(const vec2<float> &p) const {
    return _labels[button_menu].tapped(p);
}

template<> bool HomeView::tapped<button_type_goals>(const vec2<float> &p) const {
    return _labels[button_goals].tapped(p);
}

void HomeView::setHighscore(unsigned long long score) {
    if (score) {
        _highscore_alpha = 1.f;
        _labels[label_highscore].setText(
            30.f,
            screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
            std::to_wstring(score),
            alignment_type::center
        );
        _labels[label_highscore].setPosition({
            screen_size.x / 2.f, screen_size.y - ui_paragraph_margin
        });
    }
}

void HomeView::setGoals(unsigned short completed, unsigned short total) {
    _info_showing = completed == 0;
    _labels[label_goals].setText(
        20.f,
        screen_size.x - ui_button_size - 2.f * ui_margin,
        loader->localizedString("progress", completed, total),
        alignment_type::right
    );
    _labels[label_goals].setPosition({
        (screen_size.x - ui_button_size - 2.f * ui_margin) / 2.f, ui_margin + 12.f + 20.f / 2.f
    });
}

void HomeView::setGoalsAlpha(float alpha) {
    _goals_alpha = alpha;
}
