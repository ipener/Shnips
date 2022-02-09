// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SKILLSHOTS_HPP
#define SKILLSHOTS_HPP

#include "Chip.hpp"
#include "ISkillShotDelegate.hpp"

class SkillShot {
public:
    static ISkillShotDelegate      *delegate;
    static const struct base_state *state;
};


template<skill_shot_type T>
class TSkillShot : public SkillShot {
protected:
    skill_shot_type _type = T;
};

class IndirectPassSkillShot : public TSkillShot<skill_shot_type_indirect_pass> {
public:
    void passed();

    void submit();

private:
    bool _active = true;
};


class JustPassedSkillShot : public TSkillShot<skill_shot_type_just_passed> {
public:
    void passed(unsigned short i, unsigned short j);

    void submit();

private:
    constexpr static double _maximum_velocity = 10.0;

    unsigned short _passes = 0;
    unsigned short _chip_i;
    unsigned short _chip_j;
    double         _velocity = 0.0;
};


class NarrowGapSkillShot : public TSkillShot<skill_shot_type_narrow_gap> {
public:
    void passed(unsigned short i, unsigned short j);

    void submit();

private:
    constexpr static double _maximum_chip_gap = 150.0;

    bool _active = true;
};


template<unsigned short N, skill_shot_type T>
class CushionHitSkillShot : public TSkillShot<T> {
public:
    void cushionHit();

    void passed();

    void submit();

private:
    bool           _active = true;
    unsigned short _cushion_hits = 0;
};

class ThreeCushionHitSkillShot : public CushionHitSkillShot<3, skill_shot_type_three_cushion_hit> {
};

template<unsigned short N, skill_shot_type T>
class PassSkillShot : public TSkillShot<T> {
public:
    void passed();

    void submit();

private:
    unsigned short _passes = 0;
};

class FourPassSkillShot : public PassSkillShot<4, skill_shot_type_four_passes> {
};

class ChipsCloseSkillShot : public TSkillShot<skill_shot_type_chips_close> {
public:
    void passed();

    void hit();

private:
    constexpr static double _maximum_chip_distance = Chip::default_size * 4.0;

    bool _active = false;
};


class TwoPassesNoCushionSkillShot : public TSkillShot<skill_shot_type_two_passes_no_cushion> {
public:
    void passed();

    void hit();

    void cushionHit();

    void submit();

private:
    unsigned short _passes = 0;
};


class CircumventChipSkillShot : public TSkillShot<skill_shot_type_circumvent_chip> {
public:
    void passed();

    void hit();

    void update();

private:
    Chip                _chip_at_last_position;
    std::vector<double> _angles;
    std::vector<double> _directions;
    unsigned long       _active = 0;
    bool                _passed = false;
};

class HitChipsBeforePassSkillShot : public TSkillShot<skill_shot_type_hit_chips_before_pass> {
public:
    void passed();

    void hit();

    void hit(unsigned short i);

private:
    unsigned char _active = 0;
};

#endif
