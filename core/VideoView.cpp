// Copyright © 2018 Igor Pener. All rights reserved.

#include "VideoView.hpp"

#include "assets.hpp"

using namespace assets;

enum {
    button_cross,
    label_notification,
    label_notification_info,
    label_range
};

enum {
    line_notification,
    line_lower,
    line_upper,
    line_range
};

VideoView::VideoView(IViewDelegate<VideoView> *delegate) : TView<VideoView>(delegate, label_range, line_range) {
    _zoomed_in_data.x = video_view_size / 2.f;
    _zoomed_in_data.y = screen_size.y * video_view_size / screen_size.x * 0.5f;
    _zoomed_in_data.z = _zoomed_in_data.y;

    _labels[button_cross] = {
        {screen_size.x / 2.f, ui_paragraph_margin},
        ui_button_size,
        glyphs::buttons[button_type_cross],
        font_weight::thin
    };

    _lines[line_notification] = {
        {screen_size.x / 2.f, screen_size.y - ui_paragraph_margin - (30.f + 6.f) / 2.f - 12.f - 20.f - 6.f},
        screen_size.x - 2.f * ui_paragraph_margin
    };
    _lines[line_lower] = {
        {screen_size.x / 2.f, 2.f * ui_paragraph_margin},
        screen_size.x
    };
    _lines[line_upper] = {
        {screen_size.x / 2.f, _lines[line_lower].p.y + screen_size.x},
        screen_size.x
    };
}

void VideoView::setTransitionProgress(float progress) {
    progress = 1.f - fabs(progress);
    _zoom_data.x = screen_size.x / 2.f * (1.f - progress) + _zoomed_in_data.x * progress;
    _zoom_data.y = screen_size.y / 2.f * (1.f - progress) + _zoomed_in_data.y * progress;
    _zoom_data.z = screen_size.y / 2.f * (1.f - progress) + _zoomed_in_data.z * progress;

    _labels[button_cross].alpha = progress;

    _labels[label_notification].alpha = progress;
    _labels[label_notification_info].alpha = progress;

    _lines[line_notification].alpha = progress;
    _lines[line_lower].alpha = progress;
    _lines[line_upper].alpha = progress;
}

template<> bool VideoView::tapped<button_type_cross>(const vec2<float> &p) const {
    return _labels[button_cross].tapped(p);
}

void VideoView::setNotification(video_type type) {
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
