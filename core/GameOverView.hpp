// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GAMEOVERVIEW_HPP
#define GAMEOVERVIEW_HPP

#include "chip_type.h"
#include "IView.hpp"

class GameOverView : public TView<GameOverView> {
public:
    GameOverView(IViewDelegate<GameOverView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setScore(unsigned long long score, bool new_highscore_reached);

    void setLeaderboardRank(unsigned long long all_time);

    void setChipType(chip_type type, unsigned short level);

    void setNotification(chip_type type, unsigned short level);

    void setNotificationAlpha(float alpha);

    void setAuthenticated(bool authenticated);

    void setChipProgress(unsigned long long progress, unsigned long long total);

private:
    void setLeaderboardAlpha(float alpha);

private:
    float _sign_in_button_alpha = 0.f;
    float _notification_alpha = 0.f;
    bool  _authenticated = false;
    bool  _max_chip_level_reached = false;
};

#endif
