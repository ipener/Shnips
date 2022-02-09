// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef HIGHSCOREVIEW_HPP
#define HIGHSCOREVIEW_HPP

#include "IView.hpp"
#include "skill_shot_type.h"

class HighscoreView : public TView<HighscoreView> {
public:
    HighscoreView(IViewDelegate<HighscoreView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setScore(unsigned long long score);

    void setShotScore(unsigned long long score);

    void setNotification(skill_shot_type type);

    void setNotificationAlpha(float alpha);

    void setBarInfoAlpha(float alpha);

    void setButtonInfoAlpha(float alpha);

    void setSuperchargerAlpha(float alpha);

    void setHeartButtonEnabled(bool enabled);

private:
    float _notification_alpha = 0.f;
    float _bar_info_alpha = 0.f;
    float _supercharger_alpha = 0.f;
    float _button_info_alpha = 0.f;
    float _heart_alpha = 0.f;
};

#endif
