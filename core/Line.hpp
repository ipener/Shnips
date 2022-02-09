// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef LINE_HPP
#define LINE_HPP

#include "color.hpp"
#include "vec.hpp"

class Line {
public:
    Line() = default;

    Line(const vec2<float> &p, float size, float alpha = 0.f) : p(p), size(size), alpha(alpha) {
    }

    vec2<float> p;
    float       size = 0.f;
    float       alpha = 0.f;
};

class Circle {
public:
    Circle() = default;

    Circle(const vec2<float> &p, float size) : p(p), size(size) {
    }

    ::color     color = {0.f, 0.f, 0.f, 0.f};
    vec2<float> p;
    float       size = 0.f;
    float       progress = 0.f;
};

#endif
