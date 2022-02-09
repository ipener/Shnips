// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ALERTVIEW_HPP
#define ALERTVIEW_HPP

#include "IView.hpp"
#include "tutorial_type.h"
#include "intro_type.h"

class AlertView : public TView<AlertView> {
public:
    AlertView(IViewDelegate<AlertView> *delegate);

    void setTransitionProgress(float progress) override;

    view_type type() const override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setTitle(tutorial_type type);

    void setTitle(intro_type type, float origin_y);

    bool introShowing() const;

private:
    void setTitle(const std::string &key);

private:
    bool _info_visible = false;
    bool _intro_showing = false;
};


#endif
