// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef VEC_HPP
#define VEC_HPP

#include <cmath>

template<typename T>
struct vec2 {
    T x, y;

    vec2<T>() : x(0), y(0) {
    }

    vec2<T>(T x, T y) : x(x), y(y) {
    }

    T normSquared() const {
        return x * x + y * y;
    }

    T norm() const {
        return sqrt(normSquared());
    }

    void normalize() {
        T n = norm();
        x /= n;
        y /= n;
    }

    T dist(const vec2<T> &v) const {
        return (*this - v).norm();
    }

    T dot(const vec2<T> &v) const {
        return x * v.x + y * v.y;
    }

    T cross(const vec2<T> &v) const {
        return x * v.y - y * v.x;
    }

    T angle(const vec2<T> &v) const {
        T n = norm() * v.norm();
        return fabs(n) < 1e-6 ? 0.0 : acos(dot(v) / n);
    }

    vec2<T> abs() const {
        return vec2<T>(x < 0.0 ? -x : x, y < 0.0 ? -y : y);
    }

    bool operator < (T n) const {
        return x < n && y < n;
    }

    bool operator > (T n) const {
        return x > n && y > n;
    }

    bool operator == (double n) const {
        return fabs(n - x) < 1e-6 && fabs(n - y) < 1e-6;
    }

    bool operator == (float n) const {
        return fabs(n - x) < 1e-5f && fabs(n - y) < 1e-5f;
    }

    bool operator != (T n) const {
        return !this->operator==(n);
    }

    bool operator < (const vec2<T> &v) const {
        return x < v.x && y < v.y;
    }

    bool operator == (const vec2<T> &v) const {
        return fabs(v.x - x) < 1e-6 && fabs(v.y - y) < 1e-6;
    }

    bool operator != (const vec2<T> &v) const {
        return !this->operator==(v);
    }

    vec2<T> operator - (const vec2<T> &v) const {
        return vec2<T>(x - v.x, y - v.y);
    }

    vec2<T> operator + (const vec2<T> &v) const {
        return vec2<T>(x + v.x, y + v.y);
    }

    vec2<T> operator / (const vec2<T> &v) const {
        return vec2<T>(x / v.x, y / v.y);
    }

    vec2<T> operator + (T n) const {
        return vec2<T>(x + n, y + n);
    }

    vec2<T> operator - (T n) const {
        return vec2<T>(x - n, y - n);
    }

    vec2<T> operator * (T n) const {
        return vec2<T>(x * n, y * n);
    }

    vec2<T> operator / (T n) const {
        return vec2<T>(x / n, y / n);
    }

    vec2<T> &operator += (const vec2<T> &v) {
        x += v.x;
        y += v.y;
        return (*this);
    }

    vec2<T> &operator -= (const vec2<T> &v) {
        x -= v.x;
        y -= v.y;
        return (*this);
    }

    vec2<T> &operator *= (T n) {
        x *= n;
        y *= n;
        return (*this);
    }

    vec2<T> &operator /= (T n) {
        x /= n;
        y /= n;
        return (*this);
    }
};

template<typename T>
struct vec3 {
    T x, y, z;

    vec3<T>() : x(0), y(0), z(0) {
    }

    vec3<T>(T x, T y, T z) : x(x), y(y), z(z) {
    }
};

template<typename T>
struct vec4 {
    T x, y, z, w;

    vec4<T>() : x(0), y(0), z(0), w(0) {
    }

    vec4<T>(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
    }
};

#endif
