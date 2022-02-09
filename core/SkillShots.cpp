// Copyright © 2018 Igor Pener. All rights reserved.

#include "SkillShots.hpp"

#include "assets.hpp"
#include "base_state.hpp"

ISkillShotDelegate *SkillShot::delegate = nullptr;
const base_state *SkillShot::state = nullptr;

void IndirectPassSkillShot::passed() {
    if (_active) {
        if (state->chips_data[state->last_index].v == 0.0) {
            delegate->skillShotAchieved(_type);
        }
        _active = false;
    }
}

void IndirectPassSkillShot::submit() {
    _active = true;
}

void JustPassedSkillShot::passed(unsigned short i, unsigned short j) {
    if (_passes == 0) {
        _passes = 1;
        _chip_i = i;
        _chip_j = j;

        const vec2<double> v = state->chips_data[state->last_index].v.abs() +
            state->chips_data[_chip_i].v.abs() + state->chips_data[_chip_j].v.abs();
        _velocity = v.normSquared();

        if (_velocity > _maximum_velocity) {
            _passes = 0xFFFF;
        }
    } else {
        _passes = 0xFFFF;
        return;
    }
}

void JustPassedSkillShot::submit() {
    if (_passes == 1) {
        if (state->chips_data[state->last_index].collides(state->chips_data[_chip_i].p, state->chips_data[_chip_j].p)) {
            SkillShot::delegate->skillShotAchieved(_type);
        }
    }
    _passes = 0;
    _velocity = 0.0;
}

void NarrowGapSkillShot::passed(unsigned short i, unsigned short j) {
    if (_active) {
        const auto size = state->chips_data.size();
        for (size_t i = 0; i < size; ++i) {
            if (i == state->last_index)
                continue;

            if (state->chips_data[i].v != 0.0) {
                _active = false;
                return;
            }
        }
        if (state->chips_data[i].p.dist(state->chips_data[j].p) < _maximum_chip_gap) {
            SkillShot::delegate->skillShotAchieved(_type);
        }
        _active = false;
    }
}

void NarrowGapSkillShot::submit() {
    _active = true;
}

template<unsigned short N, skill_shot_type T>
void CushionHitSkillShot<N, T>::passed() {
    if (_active && _cushion_hits >= N) {
        TSkillShot<T>::SkillShot::delegate->skillShotAchieved(TSkillShot<T>::_type);
        _active = false;
    }
    _cushion_hits = 0;
}

template<unsigned short N, skill_shot_type T>
void CushionHitSkillShot<N, T>::cushionHit() {
    _cushion_hits++;
}

template<unsigned short N, skill_shot_type T>
void CushionHitSkillShot<N, T>::submit() {
    _cushion_hits = 0;
    _active = true;
}

template<unsigned short N, skill_shot_type T>
void PassSkillShot<N, T>::passed() {
    if (++_passes == N) {
        TSkillShot<T>::SkillShot::delegate->skillShotAchieved(TSkillShot<T>::_type);
    }
}

template<unsigned short N, skill_shot_type T>
void PassSkillShot<N, T>::submit() {
    _passes = 0;
}

void ChipsCloseSkillShot::passed() {
    if (_active) {
        SkillShot::delegate->skillShotAchieved(_type);
        _active = false;
    }
}

void ChipsCloseSkillShot::hit() {
    const auto size = state->chips_data.size();
    for (size_t i = 0; i < size - 1; ++i) {
        for (size_t j = i + 1; j < size; ++j) {
            if (state->chips_data[i].p.dist(state->chips_data[j].p) > _maximum_chip_distance) {
                _active = false;
                return;
            }
        }
    }
    _active = true;
}

void TwoPassesNoCushionSkillShot::passed() {
    if (_passes < 2) {
        _passes++;
    }
}

void TwoPassesNoCushionSkillShot::hit() {
    if (state->chips_data.size() != assets::default_chips_count) {
        _passes = 0xFFFF;
    }
}

void TwoPassesNoCushionSkillShot::cushionHit() {
    _passes = 0xFFFF;
}

void TwoPassesNoCushionSkillShot::submit() {
    if (_passes == 2) {
        SkillShot::delegate->skillShotAchieved(_type);
    }
    _passes = 0;
}

void CircumventChipSkillShot::passed() {
    _passed = true;
}

void CircumventChipSkillShot::hit() {
    _passed = false;
    _active = 0;
    _chip_at_last_position = state->chips_data[state->last_index];
    const auto size = state->chips_data.size();
    if (_angles.size() < size) {
        _angles.resize(size);
        _directions.resize(size);
    }
    for (size_t i = 0; i < size; ++i) {
        if (i == state->last_index)
            continue;

        _active |= (1 << i);
        _angles[i] = 0.0;
        _directions[i] = _chip_at_last_position.v.cross(state->chips_data[i].p - _chip_at_last_position.p);
    }
}

void CircumventChipSkillShot::update() {
    const auto size = state->chips_data.size();
    if (!_active || state->last_index >= size)
        return;

    const auto &c = state->chips_data[state->last_index];
    for (size_t i = 0; i < size; ++i) {
        if (i == state->last_index || !(_active & (1 << i)))
            continue;

        const double d = c.v.cross(state->chips_data[i].p - c.p);
        if (fabs(d) < 1e-9 || (_directions[i] < 0.0 && d > 0.0) || (_directions[i] > 0.0 && d < 0.0)) {
            _active &= ~(1 << i);
            continue;
        }
        _angles[i] += (c.p - state->chips_data[i].p).angle(_chip_at_last_position.p - state->chips_data[i].p);

        if (_passed && fabs(_angles[i]) > 2.0 * M_PI) {
            SkillShot::delegate->skillShotAchieved(_type);
            _active = 0;
        }
    }
    _chip_at_last_position = c;
}

void HitChipsBeforePassSkillShot::passed() {
    if (_active == 0x07) {
        SkillShot::delegate->skillShotAchieved(_type);
    }
    _active = 0;
}

void HitChipsBeforePassSkillShot::hit() {
    _active = (1 << state->last_index);
}

void HitChipsBeforePassSkillShot::hit(unsigned short i) {
    _active |= (1 << i);
}

template class CushionHitSkillShot<3, skill_shot_type_three_cushion_hit>;
template class PassSkillShot<4, skill_shot_type_four_passes>;
