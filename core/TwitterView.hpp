// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef TWITTERVIEW_HPP
#define TWITTERVIEW_HPP

#include "IView.hpp"

class TwitterView : public TView<TwitterView> {
public:
    TwitterView(IViewDelegate<TwitterView> *delegate);

    void setTransitionProgress(float progress) override;

    view_type type() const override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    bool iconTapped(const vec2<float> &p) const;
};

#endif
