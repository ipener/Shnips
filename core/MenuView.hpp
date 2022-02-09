// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef MENUVIEW_HPP
#define MENUVIEW_HPP

#include "IView.hpp"
#include "leaderboard_type.hpp"

class MenuView : public TView<MenuView> {
public:
    MenuView(IViewDelegate<MenuView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    bool leaderboardTapped(const vec2<float> &p) const;

    bool achievementsTapped(const vec2<float> &p) const;

    void setScore(unsigned long long score);

    void setHighscore(unsigned long long score);

    void setLeaderboardRank(unsigned long long rank, leaderboard_type type);

    void setAchievements(unsigned short completed, unsigned short total);

    void setAuthenticated(bool authenticated);

    void setInGame(bool in_game);

    void setMuted(bool muted);

private:
    void setLeaderboardAlpha(float alpha);

    void setMuteUnmuteButtonAlpha(float alpha);

private:
    bool _authenticated = false;
    bool _in_game = false;
    bool _muted = false;
};

#endif
