// Copyright © 2018 Igor Pener. All rights reserved.

#include "GoalsHandler.hpp"

#include <algorithm>
#include "Goals.hpp"
#include "IGoalsHandlerDelegate.hpp"
#include "utils.hpp"

using namespace std;
using namespace assets;

#define GOAL(x) [](IGoalDelegate *delegate)->IGoal * { return new x; }

GoalsHandler::GoalsHandler(const base_state *base_state, const app_state::singleplayer *state, IGoalsHandlerDelegate *delegate) :
_delegate(delegate),
_goals {
    GOAL(SuccessfulShotsGoal(delegate, 3)),
    GOAL(PassesGoal<2>(delegate, 1)),
    GOAL(CushionsGoal<1>(delegate, 2)),
    GOAL(ScoreGoal(delegate, 10)),
    GOAL(ScorePointsInNShots<1>(delegate, 3)),
    // 5 goals
    GOAL(NoCushionGoal(delegate, 1)),
    GOAL(CushionsGoal<2>(delegate, 1)),
    GOAL(PassesGoal<3>(delegate, 1)),
    GOAL(ConsecutiveShots<CushionsGoal<1>>(delegate, 3)),
    GOAL(NoCushionGoal(delegate, 2)),
    // 10 goals
    GOAL(SuccessfulShotsGoal(delegate, 5)),
    GOAL(PassesGoal<2>(delegate, 3)),
    GOAL(AchieveSkillShotGoal<skill_shot_type_three_cushion_hit>(delegate, 1)),
    GOAL(ScorePointsInNShots<3>(delegate, 10)),
    GOAL(ScoreGoal(delegate, 75)),
    // 15 goals
    GOAL(AchieveSkillShotGoal<skill_shot_type_indirect_pass>(delegate, 1)),
    GOAL(CushionsGoal<2>(delegate, 3)),
    GOAL(SuccessfulShotsGoal(delegate, 10)),
    GOAL(ScoreGoal(delegate, 100)),
    GOAL(AchieveSkillShotGoal<skill_shot_type_narrow_gap>(delegate, 1)),
    // 20 goals
    GOAL(AchieveSkillShotGoal<skill_shot_type_four_passes>(delegate, 1)),
    GOAL(ConsecutiveShots<FastShotsGoal<3000>>(delegate, 10)),
    GOAL(ConsecutiveShots<PassesGoal<2>>(delegate, 5)),
    GOAL(NoCushionGoal(delegate, 4)),
    GOAL(PassesGoal<5>(delegate, 1, chip_type_fast)),
    // 25 goals
    GOAL(AchieveSkillShotGoal<skill_shot_type_just_passed>(delegate, 1)),
    GOAL(CushionsGoal<4>(delegate, 1)),
    GOAL(ScorePointsInNShots<1>(delegate, 10)),
    GOAL(SuccessfulShotsGoal(delegate, 20)),
    GOAL(ScoreGoal(delegate, 150)),
    // 30 goals
    GOAL(AchieveSkillShotGoal<skill_shot_type_circumvent_chip>(delegate, 1)),
    GOAL(ConsecutiveShots<FastShotsGoal<2000>>(delegate, 10)),
    GOAL(AchieveNDifferentSkillShots(delegate, 3)),
    GOAL(ChargeChipsGoal(delegate, 3)),
    GOAL(ConsecutiveShots<NoCushionGoal>(delegate, 4)),
    // 35 goals
    GOAL(SuccessfulShotsGoal(delegate, 30)),
    GOAL(PassesGoal<3>(delegate, 5)),
    GOAL(ScorePointsInNShots<10>(delegate, 50)),
    GOAL(AchieveSkillShotGoal<skill_shot_type_hit_chips_before_pass>(delegate, 1)),
    GOAL(AchieveSkillShotsInOneShot(delegate, 2)),
    // 40 goals
    GOAL(ConsecutiveShots<FastShotsGoal<1000>>(delegate, 5)),
    GOAL(ChargeUpInNMoves<5>(delegate, 1)),
    GOAL(CushionsGoal<1>(delegate, 20)),
    GOAL(ConsecutiveSkillShots<skill_shot_type_range>(delegate, 3)),
    GOAL(ScoreGoal(delegate, 250)),
    // 45 goals
    GOAL(ChargeChipsGoal(delegate, 5)),
    GOAL(SkillShotsCountGoal(delegate, 10)),
    GOAL(ConsecutiveShots<FastShotsGoal<1000>>(delegate, 10)),
    GOAL(SwapTwoChips(delegate, 1)),
    GOAL(AchieveSkillShotGoal<skill_shot_type_two_passes_no_cushion>(delegate, 1)),
    // 50 goals
    GOAL(ShotsWithTheSameChip(delegate, 5)),
    GOAL(ConsecutiveShots<NoCushionGoal>(delegate, 7)),
    GOAL(ConsecutiveShots<PassesGoal<3>>(delegate, 5)),
    GOAL(CushionsGoal<3>(delegate, 10)),
    GOAL(AchieveNDifferentSkillShots(delegate, 5)),
    // 55 goals
    GOAL(SuccessfulShotsGoal(delegate, 50)),
    GOAL(ScorePointsInNShots<10>(delegate, 60)),
    GOAL(ChargeUpInNMoves<3>(delegate, 1)),
    GOAL(ConsecutiveSkillShots<skill_shot_type_three_cushion_hit>(delegate, 3)),
    GOAL(ScoreGoal(delegate, 400)),
    // 60 goals
    GOAL(NPassesNoCushionGoal<3>(delegate, 1)),
    GOAL(ConsecutiveIncrementalCushionShots(delegate, 4)),
    GOAL(PassesGoal<6>(delegate, 1)),
    GOAL(CushionsGoal<5>(delegate, 1)),
    GOAL(ScorePointsInNShots<10>(delegate, 75)),
    // 65 goals
    GOAL(ConsecutiveShots<NoCushionGoal>(delegate, 10)),
    GOAL(ScoreGoal(delegate, 500)),
    GOAL(ConsecutiveShots<PassesGoal<4>>(delegate, 4)),
    GOAL(ConsecutiveSkillShots<skill_shot_type_indirect_pass>(delegate, 2)),
    GOAL(ShotsWithTheSameChip(delegate, 10)),
    // 70 goals
    GOAL(ConsecutiveIncrementalPassesShots(delegate, 4)),
    GOAL(AchieveNDifferentSkillShots(delegate, 7)),
    GOAL(NPassesNoCushionGoal<4>(delegate, 1)),
    GOAL(SuccessfulShotsGoal(delegate, 75, chip_type_default)),
    GOAL(ScoreGoal(delegate, 750)),
    // 75 goals
} {
    IGoal::base_state = base_state;
    IGoal::state = state;
}

GoalsHandler::~GoalsHandler() {
    if (_current_goal) {
        delete _current_goal;
    }
}

void GoalsHandler::setEnabled(bool enabled) {
    _enabled = enabled;
}

void GoalsHandler::setCurrentGoal(unsigned short id) {
    if (id < assets::goals_count) {
        _current_goal = (*_goals[id])(this);
        _current_goal->setProgress(IGoal::state->progress);
        _delegate->setVideoType(_current_goal->videoType());
    }
}

void GoalsHandler::resetCurrentGoal() {
    if (_current_goal) {
        _current_goal->setProgress(0);
    }
}

void GoalsHandler::skillShotAchieved(skill_shot_type type) {
    if (goalActive()) {
        _current_goal->skillShotAchieved(type);
    }
}

void GoalsHandler::beginShot() {
    if (goalActive()) {
        _current_goal->beginShot();
    }
}

void GoalsHandler::endShot()  {
    if (goalActive()) {
        _current_goal->endShot();
    }
}

void GoalsHandler::chipsCharged() {
    if (goalActive()) {
        _current_goal->chipsCharged();
    }
}

void GoalsHandler::submit()  {
    if (goalActive()) {
        _current_goal->submit();
    }
}

void GoalsHandler::swappedChips() {
    if (goalActive()) {
        _current_goal->swappedChips();
    }
}

void GoalsHandler::addScore(unsigned short score)  {
    if (goalActive()) {
        _current_goal->addScore(score);
    }
}

unsigned long long GoalsHandler::progress() const {
    return _current_goal->progress();
}

unsigned long long GoalsHandler::count() const {
    return _current_goal->count();
}

void GoalsHandler::goalCompleted() {
    const auto id = IGoal::state->goal;
    _delegate->goalCompleted(id);
}

void GoalsHandler::setProgress(unsigned long long progress) {
    _delegate->setProgress(progress);
}

bool GoalsHandler::goalActive() const {
    return _enabled && _current_goal && _current_goal->isActive(IGoal::base_state->chip_type);
}

#undef GOAL
