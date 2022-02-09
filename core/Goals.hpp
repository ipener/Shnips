// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GOALS_HPP
#define GOALS_HPP

#include <chrono>

#include "app_state.hpp"
#include "IGoalDelegate.hpp"
#include "Logger.hpp"
#include "skill_shot_type.h"
#include "video_type.h"
#include "utils.hpp"

class IGoal {
public:
    IGoal(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range);

    virtual ~IGoal();

    virtual void skillShotAchieved(skill_shot_type type);

    virtual void beginShot();

    virtual void endShot();

    virtual void chipsCharged();

    virtual void submit();

    virtual void swappedChips();

    virtual void addScore(unsigned short score);

    virtual unsigned long long progress() const;

    virtual video_type videoType() const = 0;

    virtual bool isActive(chip_type type) const;

    virtual bool goalCompleted() const;

    void setProgress(unsigned long long progress);

    unsigned long long count() const;

protected:
    void checkAchieved();

private:
    bool chipMatches(chip_type type) const;

public:
    static const ::base_state            *base_state;
    static const app_state::singleplayer *state;

protected:
    IGoalDelegate     *_delegate = nullptr;
    chip_type          _chip_type = chip_type_range;
    unsigned long long _progress = 0;
    unsigned long long _count = 0;
};

template<video_type T = video_type_range>
class TGoal : public IGoal {
public:
    using IGoal::IGoal;

    virtual video_type videoType() const override {
        return goalCompleted() ? video_type_range : _video_type;
    }

protected:
    video_type _video_type = T;
};

class SuccessfulShotsGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void submit() override;
};

template<unsigned short MS>
class FastShotsGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void beginShot() override {
        _end_shot_time = 0;
        _begin_shot_time = utils::currentTimestampMs();
    }

    void endShot() override {
        if (_end_shot_time == 0) {
            _end_shot_time = utils::currentTimestampMs();
        }
    }

    void submit() override {
        if (base_state->shot_data.passes && _end_shot_time >= _begin_shot_time &&
            (_end_shot_time - _begin_shot_time <= MS || _begin_shot_time == 0)) {
            setProgress(_progress + 1);
            checkAchieved();
        } else {
            _begin_shot_time = 0;
            setProgress(0);
        }
    }

private:
    long long _begin_shot_time = 0;
    long long _end_shot_time = 0;
};

template<unsigned short N>
class CushionsGoal : public TGoal<> {
public:
    CushionsGoal(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        if (N == 2) {
            _video_type = video_type_double_cushion_shot;
        }
    }

    void submit() override {
        if (base_state->shot_data.passes) {
            if (base_state->shot_data.max_cushions_before_pass >= N) {
                setProgress(_progress + 1);
                checkAchieved();
            }
        } else {
            setProgress(0);
        }
    }
};

class NoCushionGoal : public TGoal<video_type_no_cushion_shot> {
public:
    using TGoal<video_type_no_cushion_shot>::TGoal;

    void submit() override;
};

template<unsigned short N>
class PassesGoal : public TGoal<> {
public:
    PassesGoal(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        if (N == 2) {
            _video_type = video_type_double_pass;
        } else if (N == 3) {
            _video_type = video_type_triple_pass;
        }
    }

    void submit() override {
        if (base_state->shot_data.passes >= N) {
            setProgress(_progress + 1);
            checkAchieved();
        } else if (base_state->shot_data.passes == 0) {
            setProgress(0);
        }
    }
};

class ScoreGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    virtual void submit() override;
};

template<unsigned short N>
class ScorePointsInNShots : public TGoal<> {
public:
    ScorePointsInNShots(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        unsigned short n = N;
        while (n > 0) {
            _bits_for_N++;
            n >>= 1;
        }
    }
    
    void submit() override {
        const auto shift = (sizeof(_progress) * 8 - _bits_for_N);
        const unsigned short n = ((_progress << shift) >> shift) + 1;
        if (base_state->shot_data.passes == 0 || n >= N) {
            setProgress(0);
        } else {
            _progress = ((_progress >> _bits_for_N) << _bits_for_N) | n;
        }
    }

    void addScore(unsigned short score) override {
        if (_delegate) {
            const auto shift = (sizeof(_progress) * 8 - _bits_for_N);
            const unsigned short n = (_progress << shift) >> shift;
            const unsigned long long progress = (_progress >> _bits_for_N) + score;
            if (progress >= _count) {
                _progress = (_count << _bits_for_N) | n;
                _delegate->setProgress(_count);
                _delegate->goalCompleted();
            } else {
                _progress = (progress << _bits_for_N) | n;
                _delegate->setProgress(progress);
            }
        }
    }

    unsigned long long progress() const override {
        return (_progress >> _bits_for_N);
    }

private:
    unsigned short _bits_for_N = 0;
};

class ChargeChipsGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void submit() override;

    void chipsCharged() override;
};

template<skill_shot_type T>
class AchieveSkillShotGoal : public TGoal<> {
public:
    AchieveSkillShotGoal(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        _count = count;
        if (T < skill_shot_type_range) {
            _video_type = static_cast<video_type>(T);
        }
    }

    void skillShotAchieved(skill_shot_type type) override {
        if (type == T) {
            setProgress(_progress + 1);
            checkAchieved();
        }
    }

    void submit() override {
        if (base_state->shot_data.passes == 0) {
            setProgress(0);
        }
    }
};

class SkillShotsCountGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void skillShotAchieved(skill_shot_type type) override;

    void submit() override;
};

class AchieveNDifferentSkillShots : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void skillShotAchieved(skill_shot_type type) override;

    void submit() override;

    unsigned long long progress() const override;

    bool goalCompleted() const override;
};

class AchieveSkillShotsInOneShot : public AchieveNDifferentSkillShots {
public:
    using AchieveNDifferentSkillShots::AchieveNDifferentSkillShots;

    void submit() override;
};

class SwapTwoChips : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void swappedChips() override;
};

class ShotsWithTheSameChip : public TGoal<> {
public:
    using TGoal<>::TGoal;
    
    void submit() override;

private:
    unsigned short _last_index;
};

template<unsigned short N>
class NChipsOnFieldGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void beginShot() override {
        if (base_state->chips_data.size() == N) {
            setProgress(1);
            checkAchieved();
        }
    }
};

template<unsigned short N>
class NPassesNoCushionGoal : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void submit() override {
        if (base_state->shot_data.passes >= N &&
            base_state->shot_data.max_cushions_before_pass == 0 &&
            base_state->shot_data.cushions_after_last_pass == 0) {
            setProgress(1);
            checkAchieved();
        }
    }
};

template<unsigned short N>
class ChargeUpInNMoves : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void submit() override {
        if (base_state->shot_data.passes == 0) {
            setProgress(0);
        } else if (IGoal::base_state->ability_time_left < 1e-5f) {
            _progress >>= 1;
            _progress++;
            setProgress(_progress << 1);
        } else {
            setProgress(0);
        }
    }

    void chipsCharged() override {
        if ((_progress >> 1) < N) {
            if (_delegate) {
                _delegate->setProgress(_progress | 1);
                _delegate->goalCompleted();
            }
        } else {
            setProgress(0);
        }
    }

    unsigned long long progress() const override {
        return _progress & 1;
    }
};

// If T is skill_shot_type_range any skill shots count towards the progress
template<skill_shot_type T>
class ConsecutiveSkillShots : public TGoal<> {
public:
    ConsecutiveSkillShots(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        if (T < skill_shot_type_range) {
            _video_type = static_cast<video_type>(T);
        }
    }

    void skillShotAchieved(skill_shot_type type) override {
        if (T == type || T == skill_shot_type_range) {
            setProgress(_progress + 1);
            checkAchieved();
        }
    }

    void submit() override {
        if (base_state->shot_data.passes == 0 || _last_progress == _progress) {
            setProgress(0);
            _last_progress = 0;
        } else if (_last_progress != _progress) {
            _last_progress = _progress;
        }
    }

private:
    unsigned long long _last_progress = 0;
};

class ConsecutiveIncrementalPassesShots : public TGoal<> {
public:
    using TGoal<>::TGoal;

    void submit() override;
};

class ConsecutiveIncrementalCushionShots : public TGoal<> {
public:
    using TGoal<>::TGoal;
    
    void submit() override;
};

template<typename T>
class ConsecutiveShots : public TGoal<> {
public:
    ConsecutiveShots(IGoalDelegate *delegate, unsigned long long count, chip_type type = chip_type_range) : TGoal<>(delegate, count, type) {
        _goal = new T(nullptr, 1, type);
    }

    ~ConsecutiveShots() {
        delete _goal;
    }

    void skillShotAchieved(skill_shot_type type) override {
        _goal->skillShotAchieved(type);
    }

    void beginShot() override {
        _goal->beginShot();
    }

    void endShot() override {
        _goal->endShot();
    }

    void chipsCharged() override {
        _goal->chipsCharged();
    }

    void submit() override {
        _goal->submit();
        if (_goal->goalCompleted()) {
            setProgress(_progress + 1);
            checkAchieved();
            *_goal = T(nullptr, 1, _chip_type);
        } else {
            setProgress(0);
        }
    }

    void swappedChips() override {
        _goal->swappedChips();
    }

    void addScore(unsigned short score) override {
        _goal->addScore(score);
    }

    bool isActive(chip_type type) const override {
        return _goal != nullptr;
    }

    video_type videoType() const override {
        return _goal->videoType();
    }

private:
    T *_goal = nullptr;
};

#endif
