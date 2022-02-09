// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef DISC_HPP
#define DISC_HPP

#include "vec.hpp"

struct disc {
    vec2<double> p, v;
    double       r;

    disc(const vec2<double> &p = {}, const vec2<double> &v = {}, double r = 0.0) : p(p), v(v), r(r) {
    }
};

#endif
