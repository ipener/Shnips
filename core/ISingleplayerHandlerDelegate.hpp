// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ISINGLEPLAYERHANDLERDELEGATE_HPP
#define ISINGLEPLAYERHANDLERDELEGATE_HPP

#include "intro_type.h"
#include "skill_shot_type.h"
#include "video_type.h"

class ISingleplayerHandlerDelegate {
public:
    virtual void setHighscore(unsigned long long score, bool post_to_leaderboard) = 0;

    virtual void setScore(unsigned long long score) = 0;

    virtual void setShotScore(unsigned long long score) = 0;

    virtual void setNotification(skill_shot_type type) = 0;

    virtual void setNotificationAlpha(float alpha) = 0;

    virtual void setUpgradeNotificationAlpha(float alpha) = 0;

    virtual void setBarInfoAlpha(float alpha) = 0;

    virtual void setButtonInfoAlpha(float alpha) = 0;

    virtual void setSuperchargerAlpha(float alpha) = 0;

    virtual void addChipProgress(unsigned long long progress) = 0;

    virtual void setProgress(unsigned long long progress, unsigned long long count) = 0;

    virtual void setGoal(unsigned short goal) = 0;

    virtual void setVideoType(video_type type) = 0;

    virtual void showHomeView() = 0;

    virtual void showHighscoreView() = 0;

    virtual void showGoalCompletedView() = 0;

    virtual void showGameOverView(unsigned long long score, bool new_highscore_reached) = 0;
    
    virtual void showIntro(intro_type type) = 0;

    virtual void updateHeartsAlpha() = 0;
};

#endif
