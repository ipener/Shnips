// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef CHIP_HPP
#define CHIP_HPP

#include <functional>

#include "chip_type.h"
#include "disc.hpp"
#include "rect.hpp"
#include "replay_state.hpp"

class Chip : public disc {
public:
    static constexpr float default_size = 25.0;

public:
    static void defaultChipPositions(
        unsigned short n,
        const rect<double> &bounds,
        const std::function<void(const vec2<double> &)> &callback
    );

    static void defaultShotState(replay_state &state);

    Chip(const vec2<double> &p = {}, const vec2<double> &v = {}, chip_type type = chip_type_default, unsigned short level = 0);

    void collideWithCue(const disc &cue);

    bool collide(Chip *chip);

    bool collide(const rect<double> &r);

    bool collides(const ::disc &disc) const;

    bool collides(const vec2<double> &p0, const vec2<double> &p1) const;

    bool collidesOnTranslation(const vec2<double> &q, const vec2<double> &to);

    double timeUntilPass(const Chip *c0, const Chip *c1) const;

    double timeToCollision(const disc &cue) const;

    double timeToCollision(const Chip *chip) const;

    double timeToCollision(const rect<double> &r) const;

public:
    chip_type      type = chip_type_default;
    unsigned short level = 0;
    wchar_t        icon = 0;
    wchar_t        gradient = 0;
};

#endif
