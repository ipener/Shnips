// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef HEARTSVIEW_HPP
#define HEARTSVIEW_HPP

#include "IView.hpp"

class HeartsView : public TView<HeartsView> {
public:
    HeartsView(IViewDelegate<HeartsView> *delegate);

    void setTransitionProgress(float progress) override;

    view_type type() const override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    bool buyButtonTapped(const vec2<float> &p) const;

    void setHeartPrice(const std::wstring &price);
};

#endif
