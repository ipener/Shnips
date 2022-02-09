// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef RECT_HPP
#define RECT_HPP

#include "vec.hpp"

template<typename T>
struct rect {
    vec2<T> origin, size;

    rect(const vec2<T> &origin = {}, const vec2<T> &size = {}) : origin(origin), size(size) {
    }
};

#endif
