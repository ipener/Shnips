// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GOALSHANDLER_HPP
#define GOALSHANDLER_HPP

#include "app_state.hpp"
#include "assets.hpp"
#include "IGoalDelegate.hpp"
#include "skill_shot_type.h"

class GoalsHandler : public IGoalDelegate {
public:
    GoalsHandler(const base_state *base_state, const app_state::singleplayer *state, class IGoalsHandlerDelegate *delegate);

    ~GoalsHandler();

    void setEnabled(bool enabled);

    void setCurrentGoal(unsigned short id);

    void resetCurrentGoal();

    void skillShotAchieved(skill_shot_type type);

    void beginShot();

    void endShot();

    void supercharge();

    void chipsCharged();

    void submit();

    void swappedChips();

    void addScore(unsigned short score);

    unsigned long long progress() const;

    unsigned long long count() const;

private:
    void goalCompleted() override;

    void setProgress(unsigned long long progress) override;

    bool goalActive() const;

    class IGoal *(* const _goals[assets::goals_count])(IGoalDelegate *);
    class IGoalsHandlerDelegate *_delegate = nullptr;
    class IGoal                 *_current_goal = nullptr;
    bool                         _enabled = false;
};

#endif
