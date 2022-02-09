// Copyright © 2018 Igor Pener. All rights reserved.

#include "GameOverView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_tick,
    button_share,
    button_sign_in,
    label_notification,
    label_notification_info,
    label_score,
    label_score_info,
    label_leaderboard_all_time,
    label_leaderboard_all_time_info,
    label_unlockable,
    label_progress,
    label_range
};

enum {
    line_notification,
    line_score,
    line_leaderboard_all_time,
    line_range
};

enum {
    circle_progress,
    circle_range
};

GameOverView::GameOverView(IViewDelegate<GameOverView> *delegate) : TView<GameOverView>(delegate, label_range, line_range, circle_range) {
    _labels[button_share] = {
        {screen_size.x / 2.f - 2.f * ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_share],
#ifdef __APPLE__ // Note: the share button looks different on iOS and Android
        font_weight::thin
#else
        font_weight::regular
#endif
    };
    _labels[button_tick] = {
        {screen_size.x / 2.f + 2.f * ui_button_size, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_tick],
        font_weight::thin
    };
    _labels[button_sign_in] = {
        {screen_size.x / 2.f, (screen_size.y - 48.f * 4.f) / 4.f},
        ui_button_size,
        glyphs::buttons[button_type_sign_in],
        font_weight::thin
    };

    _labels[label_score_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString("score"),
        alignment_type::left,
        font_weight::thick
    };
    _labels[label_score_info].setPosition({
        ui_paragraph_margin + _labels[label_score_info].size.x / 2.f,
        (screen_size.y + 48.f * 4.f) / 2.f + _labels[label_score_info].size.y / 2.f
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
        _labels[label_score_info].p.y - 48.f
    });

    _lines[line_score] = {
        {screen_size.x / 2.f, (screen_size.y + 48.f * 4.f) / 2.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
    _lines[line_leaderboard_all_time] = {
        {_lines[line_score].p.x, _lines[line_score].p.y - 48.f},
        _lines[line_score].size
    };
    _lines[line_notification] = {
        {screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (30.f + 6.f) / 2.f - 12.f - 20.f - 6.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };

    _circles[circle_progress] = {
        {screen_size.x / 2.f, _lines[line_leaderboard_all_time].p.y - 2.f * 48.f},
        48.f
    };

    setLeaderboardRank(0);
}

void GameOverView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    _blur_data.z = progress;

    _labels[button_tick].alpha = progress;
    _labels[button_share].alpha = progress;
    _labels[button_sign_in].alpha = progress;

    const auto notification_alpha = progress * _notification_alpha;
    _labels[label_notification].alpha = notification_alpha;
    _labels[label_notification_info].alpha = notification_alpha;
    _labels[label_score].alpha = progress;
    _labels[label_score_info].alpha = progress;
    _lines[line_notification].alpha = notification_alpha;
    _lines[line_score].alpha = progress;
    setLeaderboardAlpha(_authenticated ? progress : progress * ui_disabled_alpha);

    _labels[label_unlockable].alpha = progress * ui_disabled_alpha;
    if (_max_chip_level_reached) {
        _labels[label_progress].alpha = _circles[circle_progress].color.a = progress * ui_disabled_alpha;
    } else {
        _labels[label_progress].alpha = _circles[circle_progress].color.a = progress;
    }
}

template<> bool GameOverView::tapped<button_type_share>(const vec2<float> &p) const {
    return _labels[button_share].tapped(p);
}

template<> bool GameOverView::tapped<button_type_sign_in>(const vec2<float> &p) const {
    return _labels[button_sign_in].tapped(p);
}

template<> bool GameOverView::tapped<button_type_tick>(const vec2<float> &p) const {
    return _labels[button_tick].tapped(p);
}

void GameOverView::setScore(unsigned long long score, bool new_highscore_reached) {
    const auto h = 60.f;
    const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_score_info].size.x - ui_margin;
    const auto dp = (h - _labels[label_score_info].size.y) * ui_font_fill_ratio / 2.f;
    _labels[label_score].setText(
        h,
        w,
        std::to_wstring(score),
        alignment_type::right,
        font_weight::thin
    );
    _labels[label_score].setPosition({
        screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_score_info].p.y + dp
    });

    _labels[label_score_info] = {
        20.f,
        screen_size.x - 2.f * ui_paragraph_margin,
        fill_type::regular,
        loader->localizedString(new_highscore_reached ? "new_highscore" : "score"),
        alignment_type::left,
        font_weight::regular
    };
    _labels[label_score_info].setPosition({
        ui_paragraph_margin + _labels[label_score_info].size.x / 2.f,
        (screen_size.y + 48.f * 4.f) / 2.f + _labels[label_score_info].size.y / 2.f
    });
}

void GameOverView::setLeaderboardRank(unsigned long long all_time) {
    const auto h = 40.f;
    const auto dp = (h - _labels[label_leaderboard_all_time_info].size.y) * ui_font_fill_ratio / 2.f;
    const auto w = screen_size.x - 2.f * ui_paragraph_margin - _labels[label_leaderboard_all_time_info].size.x - ui_margin;
    _labels[label_leaderboard_all_time].setText(
        h,
        w,
        all_time ? std::to_wstring(all_time) : L"—",
        alignment_type::right,
        font_weight::thin
    );
    _labels[label_leaderboard_all_time].setPosition({
        screen_size.x - ui_paragraph_margin - w / 2.f, _labels[label_leaderboard_all_time_info].p.y + dp
    });
}

void GameOverView::setChipType(chip_type type, unsigned short level) {
    if (level == max_chip_level) {
        _max_chip_level_reached = true;
        _labels[label_progress].alpha = _circles[circle_progress].color.a = ui_disabled_alpha;
    } else {
        _max_chip_level_reached = false;
        _labels[label_progress].alpha = _circles[circle_progress].color.a = 1.f;
    }
    wchar_t glyph;
    font_weight weight;
    if (level == max_chip_level) {
        glyph = glyphs::tick;
        weight = font_weight::thin;
    } else {
        glyph = glyphs::chips[type];
        weight = static_cast<font_weight>(level);
    }
    if (type == chip_type_default) {
        _labels[label_unlockable].setText(
            {screen_size.x / 2.f, _lines[line_leaderboard_all_time].p.y - 48.f * 2.f},
            48.f,
            glyph,
            weight
        );
    } else {
        _labels[label_unlockable].setText(
            {screen_size.x / 2.f, _lines[line_leaderboard_all_time].p.y - 48.f * 2.f},
            48.f,
            glyph,
            weight
        );
    }
}

void GameOverView::setNotification(chip_type type, unsigned short level) {
    if (type == chip_type_default) {
        auto level_str = std::to_string(level);
        std::string key = "goals_unlocked";
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
        key = loader->key(type) + "_" + level_str;
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
    } else {
        auto key = loader->key(type);
        _labels[label_notification] = {
            20.f,
            screen_size.x - 2.f * ui_margin,
            fill_type::regular,
            loader->localizedString("upgraded", loader->localizedString(key.c_str()).c_str()),
            alignment_type::center
        };
        _labels[label_notification].setPosition({
            screen_size.x / 2.f,
            screen_size.y - ui_paragraph_margin - 12.f - (_labels[label_notification].size.y + 30.f) / 2.f - 6.f
        });
        key += "_" + std::to_string(level);
        _labels[label_notification_info] = {
            12.f,
            screen_size.x - 2.f * ui_margin,
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
}

void GameOverView::setNotificationAlpha(float alpha) {
    _notification_alpha = alpha;
    _labels[label_notification].alpha = _notification_alpha;
    _labels[label_notification_info].alpha = _notification_alpha;
    _lines[line_notification].alpha = _notification_alpha;
}

void GameOverView::setAuthenticated(bool authenticated) {
    _authenticated = authenticated;
    if (_authenticated) {
        setLeaderboardAlpha(1.f);
    } else {
        setLeaderboardAlpha(ui_disabled_alpha);
    }
}

void GameOverView::setChipProgress(unsigned long long progress, unsigned long long total) {
    if (total > 0) {
        const auto h = 16.f;
        _labels[label_progress].setText(
            h,
            screen_size.x / 2.f - 2.f * ui_paragraph_margin,
            loader->localizedString("progress", progress, total),
            alignment_type::center,
            font_weight::thick
        );
        _labels[label_progress].setPosition({
            screen_size.x / 2.f, _lines[line_leaderboard_all_time].p.y - 3.f * 48.f - ui_margin - h / 2.f
        });
    } else {
        _labels[label_progress].clearText();
    }
    _circles[circle_progress].progress = progress / static_cast<float>(total);
}

void GameOverView::setLeaderboardAlpha(float alpha) {
    _labels[label_leaderboard_all_time].alpha = alpha;
    _labels[label_leaderboard_all_time_info].alpha = alpha;
    _lines[line_leaderboard_all_time].alpha = alpha;
}
