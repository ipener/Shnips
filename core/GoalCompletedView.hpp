// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GOALCOMPLETEDVIEW_HPP
#define GOALCOMPLETEDVIEW_HPP

#include "IView.hpp"

class GoalCompletedView : public TView<GoalCompletedView> {
public:
    GoalCompletedView(IViewDelegate<GoalCompletedView> *delegate);

    void setTransitionProgress(float progress) override;

    view_type type() const override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setProgress(unsigned short progress, unsigned short total);
};

#endif
