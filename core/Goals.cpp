// Copyright © 2018 Igor Pener. All rights reserved.

#include "Goals.hpp"

const ::base_state *IGoal::base_state = nullptr;
const app_state::singleplayer *IGoal::state = nullptr;

IGoal::IGoal(IGoalDelegate *delegate, unsigned long long count, chip_type type) : _delegate(delegate), _count(count), _chip_type(type) {
    _count = count;
    _chip_type = type;
}

IGoal::~IGoal() {
}

void IGoal::skillShotAchieved(skill_shot_type type) {
}

void IGoal::beginShot() {
}

void IGoal::endShot() {
}

void IGoal::chipsCharged() {
}

void IGoal::submit() {
}

void IGoal::swappedChips() {
}

void IGoal::addScore(unsigned short score) {
}

unsigned long long IGoal::progress() const {
    return _progress;
}

bool IGoal::isActive(chip_type current_chip) const {
    return !goalCompleted() && chipMatches(current_chip);
}

bool IGoal::goalCompleted() const {
    return progress() >= _count;
}

void IGoal::setProgress(unsigned long long progress) {
    _progress = progress;
    if (_delegate) {
        _delegate->setProgress(_progress);
    }
}

unsigned long long IGoal::count() const {
    return _count;
}

void IGoal::checkAchieved() {
    if (_delegate && goalCompleted()) {
        _delegate->goalCompleted();
    }
}

bool IGoal::chipMatches(chip_type current_chip) const {
    return _chip_type == chip_type_range || _chip_type == current_chip;
}

void SuccessfulShotsGoal::submit() {
    if (base_state->shot_data.passes > 0) {
        setProgress(_progress + 1);
        checkAchieved();
    } else {
        setProgress(0);
    }
}

void NoCushionGoal::submit() {
    if (base_state->shot_data.passes) {
        if (base_state->shot_data.max_cushions_before_pass == 0 && base_state->shot_data.cushions_after_last_pass == 0) {
            setProgress(_progress + 1);
            checkAchieved();
        }
    } else {
        setProgress(0);
    }
}

void ScoreGoal::submit() {
    if (base_state->shot_data.passes == 0) {
        setProgress(0);
    } else if (state->score >= _count) {
        setProgress(_count);
        checkAchieved();
    } else {
        setProgress(state->score);
    }
}

void ChargeChipsGoal::submit() {
    if (base_state->shot_data.passes == 0) {
        setProgress(0);
    }
}

void ChargeChipsGoal::chipsCharged() {
    setProgress(_progress + 1);
    checkAchieved();
}

void SkillShotsCountGoal::skillShotAchieved(skill_shot_type type) {
    setProgress(_progress + 1);
    checkAchieved();
}

void SkillShotsCountGoal::submit() {
    if (base_state->shot_data.passes == 0) {
        setProgress(0);
    }
}

void AchieveNDifferentSkillShots::skillShotAchieved(skill_shot_type type) {
    _progress |= (1 << type);
    if (_delegate) {
        _delegate->setProgress(_progress);
    }
    checkAchieved();
}

void AchieveNDifferentSkillShots::submit() {
    if (base_state->shot_data.passes == 0) {
        setProgress(0);
    }
}

unsigned long long AchieveNDifferentSkillShots::progress() const {
    return utils::countBits(_progress);
}

bool AchieveNDifferentSkillShots::goalCompleted() const {
    return utils::countBits(_progress) >= _count;
}

void AchieveSkillShotsInOneShot::submit() {
    setProgress(0);
}

void SwapTwoChips::swappedChips() {
    setProgress(_progress + 1);
    checkAchieved();
}

void ShotsWithTheSameChip::submit() {
    if (base_state->shot_data.passes == 0) {
        setProgress(0);
    } else if (_progress == 0) {
        _last_index = base_state->last_index;
        setProgress(1);
    } else if (_last_index == base_state->last_index) {
        setProgress(_progress + 1);
        checkAchieved();
    } else {
        _last_index = base_state->last_index;
        setProgress(0);
    }
}

void ConsecutiveIncrementalPassesShots::submit() {
    if (base_state->shot_data.passes == _progress + 1) {
        setProgress(_progress + 1);
        checkAchieved();
    } else {
        setProgress(base_state->shot_data.passes == 1);
    }
}

void ConsecutiveIncrementalCushionShots::submit() {
    if (base_state->shot_data.max_cushions_before_pass == _progress + 1 &&
        base_state->shot_data.passes == 1) {
        setProgress(_progress + 1);
        checkAchieved();
    } else {
        setProgress(base_state->shot_data.max_cushions_before_pass == 1 & base_state->shot_data.passes == 1);
    }
}
