// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef HOMEVIEW_HPP
#define HOMEVIEW_HPP

#include "IView.hpp"

class HomeView : public TView<HomeView> {
public:
    HomeView(IViewDelegate<HomeView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setHighscore(unsigned long long score);

    void setGoals(unsigned short completed, unsigned short total);

    void setGoalsAlpha(float alpha);

private:
    float _highscore_alpha = 0.f;
    float _goals_alpha = 0.f;
    bool  _info_showing = false;
};

#endif
