// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef TUTORIALVIEW_HPP
#define TUTORIALVIEW_HPP

#include "IView.hpp"
#include "tutorial_type.h"

class TutorialView : public TView<TutorialView> {
public:
    TutorialView(IViewDelegate<TutorialView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setScore(unsigned long long score);

    void setShotScore(unsigned long long score);

    void setNotification(tutorial_type type);

    void setNotificationAlpha(float alpha);

    void setCancelable(bool cancelable);

private:
    float _notification_alpha = 0.f;
    bool  _cancelable = false;
};

#endif
