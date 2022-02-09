// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GOALSVIEW_HPP
#define GOALSVIEW_HPP

#include "IView.hpp"

class GoalsView : public TView<GoalsView> {
public:
    GoalsView(IViewDelegate<GoalsView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setProgress(unsigned long long progress, unsigned long long count);

    void setNotification(skill_shot_type type);

    void setNotificationAlpha(float alpha);

    void setButtonInfoAlpha(float alpha);

    void setGoal(unsigned short goal);

    void setVideoButtonEnabled(bool enabled);

    void setHeartButtonEnabled(bool enabled);

private:
    float _notification_alpha = 0.f;
    float _button_info_alpha = 0.f;
    float _heart_alpha = 0.f;
};

#endif
