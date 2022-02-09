// Copyright © 2018 Igor Pener. All rights reserved.

#include "MenuView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_cross,
    button_exit,
    button_tutorial,
    button_unmute,
    button_mute,
    button_sign_in,
    button_download,
    label_score,
    label_highscore,
    label_highscore_info,
    label_leaderboard_all_time,
    label_leaderboard_all_time_info,
    label_leaderboard_week,
    label_leaderboard_week_info,
    label_leaderboard_today,
    label_leaderboard_today_info,
    label_achievements,
    label_achievements_info,
    label_sign_in_info,
    label_restore_purchases_info,
    label_range
};

enum {
    line_highscore,
    line_leaderboard_all_time,
    line_leaderboard_week,
    line_leaderboard_today,
    line_achievements,
    line_range
};

MenuView::MenuView(IViewDelegate<MenuView> *delegate) : TView<MenuView>(delegate, label_range, line_range) {
    _labels[button_cross] = {
        {screen_size.x - ui_paragraph_margin, screen_size.y - ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin
    };
    _labels[button_exit] = {
        {screen_size.x / 2.f - 3.f * ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_exit],
        font_weight::thin
    };
    _labels[button_tutorial] = {
        _labels[button_exit].p,
        ui_button_size,
        glyphs::buttons[button_type_tutorial],
        font_weight::thin
    };
    _labels[button_unmute] = {
        {screen_size.x / 2.f + 3.f * ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_unmute],
        font_weight::thin
    };
    _labels[button_mute] = {
        _labels[button_unmute].p,
        ui_button_size,
        glyphs::buttons[button_type_mute],
        font_weight::thin
    };
    _labels[button_sign_in] = {
        {screen_size.x / 2.f - ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_sign_in],
        font_weight::thin
    };
    _labels[button_download] = {
        {screen_size.x / 2.f + ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_download],
        font_weight::thin
    };

    _labels[label_highscore_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("highscore"),
        alignment_type::left,
        font_weight::regular
    };
    _labels[label_highscore_info].setPosition({
        ui_paragraph_margin + _labels[label_highscore_info].size.x / 2.f,
        (screen_size.y + 48.f * 4.f) / 2.f + _labels[label_highscore_info].size.y / 2.f
    });

    _labels[label_leaderboard_all_time_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("leaderboard_all_time_info", glyphs::buttons[button_type_goals]),
        alignment_type::left
    };
    _labels[label_leaderboard_all_time_info].setPosition({
        ui_paragraph_margin + _labels[label_leaderboard_all_time_info].size.x / 2.f,
        _labels[label_highscore_info].p.y - 48.f
    });

    _labels[label_leaderboard_week_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("leaderboard_week_info"),
        alignment_type::left
    };
    _labels[label_leaderboard_week_info].setPosition({
        ui_paragraph_margin + 2.f * ui_margin + _labels[label_leaderboard_week_info].size.x / 2.f,
        _labels[label_leaderboard_all_time_info].p.y - 48.f
    });

    _labels[label_leaderboard_today_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("leaderboard_today_info"),
        alignment_type::left
    };
    _labels[label_leaderboard_today_info].setPosition({
        ui_paragraph_margin + 2.f * ui_margin + _labels[label_leaderboard_today_info].size.x / 2.f,
        _labels[label_leaderboard_week_info].p.y - 48.f
    });

    _labels[label_achievements_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("achievements_info", glyphs::buttons[button_type_goals]),
        alignment_type::left
    };
    _labels[label_achievements_info].setPosition({
        ui_paragraph_margin + _labels[label_achievements_info].size.x / 2.f,
        _labels[label_leaderboard_today_info].p.y - 48.f
    });
    _labels[label_sign_in_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::rounded,
        loader->localizedString("leaderboard_info"),
        alignment_type::left,
        font_weight::thick
    };
    _labels[label_sign_in_info].bounds.size = {
        _labels[label_sign_in_info].size.x + 3.f * ui_margin, _labels[label_sign_in_info].size.y + ui_margin
    };
    _labels[label_sign_in_info].bounds.origin = {
        - (_labels[label_sign_in_info].bounds.size.x - _labels[label_sign_in_info].size.x) / 2.f,
        - (_labels[label_sign_in_info].bounds.size.y - _labels[label_sign_in_info].size.y) / 2.f
    };
    _labels[label_sign_in_info].setPosition({
        screen_size.x / 2.f,
        _labels[button_sign_in].p.y + (_labels[button_sign_in].size.y + _labels[label_sign_in_info].bounds.size.y) / 2.f + 2.f * ui_margin
    });
    _labels[label_sign_in_info].pointer = 0.5f - (ui_button_size - _labels[label_sign_in_info].bounds.size.y / 6.f) / _labels[label_sign_in_info].bounds.size.x;

    _labels[label_restore_purchases_info] = {
        12.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::rounded,
        loader->localizedString("restore_purchases"),
        alignment_type::left,
        font_weight::thick
    };
    _labels[label_restore_purchases_info].bounds.size = {
        _labels[label_restore_purchases_info].size.x + 3.f * ui_margin, _labels[label_restore_purchases_info].size.y + ui_margin
    };
    _labels[label_restore_purchases_info].bounds.origin = {
        - (_labels[label_restore_purchases_info].bounds.size.x - _labels[label_restore_purchases_info].size.x) / 2.f,
        - (_labels[label_restore_purchases_info].bounds.size.y - _labels[label_restore_purchases_info].size.y) / 2.f
    };
    _labels[label_restore_purchases_info].setPosition({
        screen_size.x / 2.f,
        _labels[button_sign_in].p.y - (_labels[button_sign_in].size.y + _labels[label_restore_purchases_info].bounds.size.y) / 2.f - 2.f * ui_margin
    });
    _labels[label_restore_purchases_info].pointer = - (0.5f + (ui_button_size - _labels[label_restore_purchases_info].bounds.size.y / 6.f) / _labels[label_restore_purchases_info].bounds.size.x);

    _lines[line_highscore] = {
        {screen_size.x / 2.f, (screen_size.y + 48.f * 4.f) / 2.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
    _lines[line_leaderboard_all_time] = {
        {_lines[line_highscore].p.x, _lines[line_highscore].p.y - 48.f},
        _lines[line_highscore].size
    };
    _lines[line_leaderboard_week] = {
        {_lines[line_leaderboard_all_time].p.x + ui_margin, _lines[line_leaderboard_all_time].p.y - 48.f},
        _lines[line_leaderboard_all_time].size - 2.f * ui_margin
    };
    _lines[line_leaderboard_today] = {
        {_lines[line_leaderboard_week].p.x, _lines[line_leaderboard_week].p.y - 48.f},
        _lines[line_leaderboard_week].size
    };
    _lines[line_achievements] = {
        {_lines[line_highscore].p.x, _lines[line_leaderboard_today].p.y - 48.f},
        _lines[line_highscore].size
    };

    setLeaderboardRank(0, leaderboard_type::all_time);
    setLeaderboardRank(0, leaderboard_type::week);
    setLeaderboardRank(0, leaderboard_type::today);
}

void MenuView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    _blur_data.z = progress;

    _labels[button_cross].alpha = progress;
    _labels[button_sign_in].alpha = progress;
    if (_in_game) {
        _labels[button_exit].alpha = progress;
        _labels[button_tutorial].alpha = 0.f;
        _labels[label_score].alpha = progress;
    } else {
        _labels[button_exit].alpha = 0.f;
        _labels[button_tutorial].alpha = progress;
        _labels[label_score].alpha = 0.f;
    }
#ifdef __APPLE__
    _labels[label_restore_purchases_info].alpha = progress;
    _labels[button_download].alpha = progress;
#else
    _labels[label_restore_purchases_info].alpha = 0.f;
    _labels[button_download].alpha = progress * ui_disabled_alpha;
#endif
    _labels[label_highscore].alpha = progress;
    _labels[label_highscore_info].alpha = progress;
    _lines[line_highscore].alpha = progress;
    _labels[label_sign_in_info].alpha = _authenticated ? 0.f : progress;
    setLeaderboardAlpha(_authenticated ? progress : progress * ui_disabled_alpha);
    setMuteUnmuteButtonAlpha(progress);
}

template<> bool MenuView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

template<> bool MenuView::tapped<button_type_exit>(const vec2<float> &p) const {
    return _labels[button_exit].tapped(p);
}

template<> bool MenuView::tapped<button_type_tutorial>(const vec2<float> &p) const {
    return _labels[button_tutorial].tapped(p);
}

template<> bool MenuView::tapped<button_type_unmute>(const vec2<float> &p) const {
    return _labels[button_unmute].tapped(p);
}

template<> bool MenuView::tapped<button_type_mute>(const vec2<float> &p) const {
    return _labels[button_mute].tapped(p);
}

template<> bool MenuView::tapped<button_type_sign_in>(const vec2<float> &p) const {
    return _labels[button_sign_in].tapped(p);
}

template<> bool MenuView::tapped<button_type_download>(const vec2<float> &p) const {
    return _labels[button_download].tapped(p);
}

bool MenuView::leaderboardTapped(const vec2<float> &p) const {
    return (
        _labels[label_leaderboard_all_time].tapped(p) || _labels[label_leaderboard_all_time_info].tapped(p) ||
        _labels[label_leaderboard_week].tapped(p) || _labels[label_leaderboard_week_info].tapped(p) ||
        _labels[label_leaderboard_today].tapped(p) || _labels[label_leaderboard_today_info].tapped(p)
    );
}

bool MenuView::achievementsTapped(const vec2<float> &p) const {
    return _labels[label_achievements].tapped(p) || _labels[label_achievements_info].tapped(p);
}

void MenuView::setScore(unsigned long long score) {
    _labels[label_score].setText(
        30.f,
        screen_size.x - 2.f * (ui_button_size + 2.f * ui_margin),
        std::to_wstring(score),
        alignment_type::center
    );
    _labels[label_score].setPosition({screen_size.x / 2.f, screen_size.y - ui_paragraph_margin});
}

void MenuView::setHighscore(unsigned long long score) {
    const auto h = 60.f;
    const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_highscore_info].size.x - ui_margin;
    const auto dp = (h - _labels[label_highscore_info].size.y) * ui_font_fill_ratio / 2.f;
    _labels[label_highscore].setText(
        h,
        w,
        std::to_wstring(score),
        alignment_type::right,
        font_weight::thin
    );
    _labels[label_highscore].setPosition({
        screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_highscore_info].p.y + dp
    });
}

void MenuView::setLeaderboardRank(unsigned long long rank, leaderboard_type type) {
    const auto h = 40.f;
    const auto dp = (h - _labels[label_leaderboard_all_time_info].size.y) * ui_font_fill_ratio / 2.f;
    if (type == leaderboard_type::all_time) {
        const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_leaderboard_all_time_info].size.x - ui_margin;
        _labels[label_leaderboard_all_time].setText(
            h,
            w,
            rank ? std::to_wstring(rank) : L"—",
            alignment_type::right,
            font_weight::thin
        );
        _labels[label_leaderboard_all_time].setPosition({
            screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_leaderboard_all_time_info].p.y + dp
        });
    } else if (type == leaderboard_type::week) {
        const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_leaderboard_week_info].size.x - ui_margin;
        _labels[label_leaderboard_week].setText(
            h,
            w,
            rank ? std::to_wstring(rank) : L"—",
            alignment_type::right,
            font_weight::thin
        );
        _labels[label_leaderboard_week].setPosition({
            screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_leaderboard_week_info].p.y + dp
        });
    } else if (type == leaderboard_type::today) {
        const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_leaderboard_today_info].size.x - ui_margin;
        _labels[label_leaderboard_today].setText(
            h,
            w,
            rank ? std::to_wstring(rank) : L"—",
            alignment_type::right,
            font_weight::thin
        );
        _labels[label_leaderboard_today].setPosition({
            screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_leaderboard_today_info].p.y + dp
        });
    }
}

void MenuView::setAchievements(unsigned short completed, unsigned short total) {
    const auto h = 40.f;
    const auto dp = (h - _labels[label_achievements_info].size.y) * ui_font_fill_ratio / 2.f;
    auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_achievements_info].size.x - ui_margin;
    _labels[label_achievements].setText(
        h,
        w,
        loader->localizedString("progress", completed, total),
        alignment_type::right,
        font_weight::thin
    );
    _labels[label_achievements].setPosition({
        screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_achievements_info].p.y + dp
    });
}

void MenuView::setAuthenticated(bool authenticated) {
    _authenticated = authenticated;
    if (_authenticated) {
        _labels[label_sign_in_info].alpha = 0.f;
        setLeaderboardAlpha(1.f);
    } else {
        _labels[label_sign_in_info].alpha = 1.f;
        setLeaderboardAlpha(ui_disabled_alpha);
    }
}

void MenuView::setInGame(bool in_game) {
    _in_game = in_game;
}

void MenuView::setMuted(bool muted) {
    _muted = muted;
    setMuteUnmuteButtonAlpha(1.f);
}

void MenuView::setLeaderboardAlpha(float alpha) {
    _labels[label_leaderboard_all_time].alpha = alpha;
    _labels[label_leaderboard_all_time_info].alpha = alpha;
    _labels[label_leaderboard_week].alpha = alpha;
    _labels[label_leaderboard_week_info].alpha = alpha;
    _labels[label_leaderboard_today].alpha = alpha;
    _labels[label_leaderboard_today_info].alpha = alpha;
    _labels[label_achievements].alpha = alpha;
    _labels[label_achievements_info].alpha = alpha;
    _lines[line_leaderboard_all_time].alpha = alpha;
    _lines[line_leaderboard_week].alpha = alpha;
    _lines[line_leaderboard_today].alpha = alpha;
    _lines[line_achievements].alpha = alpha;
}

void MenuView::setMuteUnmuteButtonAlpha(float alpha) {
    if (_muted) {
        _labels[button_unmute].alpha = alpha;
        _labels[button_mute].alpha = 0.f;
    } else {
        _labels[button_unmute].alpha = 0.f;
        _labels[button_mute].alpha = alpha;
    }
}
