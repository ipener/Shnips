// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef VIDEOVIEW_HPP
#define VIDEOVIEW_HPP

#include "IView.hpp"
#include "video_type.h"

class VideoView : public TView<VideoView> {
public:
    VideoView(IViewDelegate<VideoView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type> bool tapped(const vec2<float> &p) const;

    void setNotification(video_type type);

private:
    vec3<float> _zoomed_in_data;
};

#endif
