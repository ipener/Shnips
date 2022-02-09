// Copyright © 2018 Igor Pener. All rights reserved.

#include "Chip.hpp"

#include "assets.hpp"

using namespace assets;

void Chip::defaultChipPositions(
        unsigned short n,
        const rect<double> &bounds,
        const std::function<void(const vec2<double> &)> &callback
) {
    const float delta = 6.28318530718 / static_cast<double>(n);
    const float r = std::min(120.0, (bounds.size.x - 2 * Chip::default_size) / 3.f);
    const vec2<double> offset = bounds.origin + bounds.size / 2.f;

    for (unsigned short i = 0; i < n; ++i) {
        callback(offset + vec2<double>(cos(delta * i) * r, sin(delta * i) * r));
    }
}

void Chip::defaultShotState(replay_state &state) {
    Chip::defaultChipPositions(default_chips_count, {{}, {screen_size.x, screen_size.y}},
        [&state](const vec2<double> &p) { state.chip_positions.push_back(p); }
    );
    vec2<double> p(1e100, 1e100);
    for (const auto &q : state.chip_positions) {
        p.x = std::min(p.x, q.x);
        p.y = std::min(p.y, q.y);
    }
    // The trajectory of the hit chip should be at 60deg, so x/y = tan(60) = sqrt(3)
    state.cue.r = Chip::default_size;
    state.cue.p = {p.x / 2.0, p.y - p.x / 2.0 * sqrt(3.0)};
    const auto d = p - state.cue.p;
    const double l = sqrt(d.x * d.x + d.y * d.y);
    state.cue.v = {l * 0.5, sqrt(3.0) * l * 0.5};
}

Chip::Chip(const vec2<double> &p, const vec2<double> &v, chip_type type, unsigned short level) : disc(p, v), type(type), level(level) {
    r = Chip::default_size;
}

void Chip::collideWithCue(const disc &cue) {
    const vec2<double> u = (p - cue.p) / (r + cue.r);
    v = u * cue.v.dot(u);
}

bool Chip::collide(Chip *chip) {
    vec2<double> u = p - chip->p;
    double dr = (r + chip->r);

    if (u.dot(chip->v - v) < 1e-9 || u.normSquared() > dr * dr + 1e-9)
        return false;

    u /= dr;
    u *= (v.dot(u) - chip->v.dot(u));   // d = 2.0 * (v0.dot(v) - v1.dot(v)) / (m0 + m1)
    v -= u;                             // -= v * d * m1
    chip->v += u;                       // += v * d * m0
    return true;
}

bool Chip::collide(const rect<double> &r) {
    bool did_collide = false;

    if ((v.x < 0.0 && p.x - this->r < r.origin.x + 1e-7) || (v.x > 0.0 && p.x + this->r > r.origin.x + r.size.x - 1e-7)) {
        v.x *= -0.75;
        did_collide = true;
    }
    if ((v.y < 0.0 && p.y - this->r < r.origin.y + 1e-7) || (v.y > 0.0 && p.y + this->r > r.origin.y + r.size.y - 1e-7)) {
        v.y *= -0.75;
        did_collide = true;
    }
    return did_collide;
}

bool Chip::collides(const ::disc &disc) const {
    return (p - disc.p).normSquared() < (r + disc.r) * (r + disc.r);
}

bool Chip::collides(const vec2<double> &p0, const vec2<double> &p1) const {
    vec2<double> min, max;

    if (p0.x < p1.x) {
        min.x = p0.x - r;
        max.x = p1.x + r;
    } else {
        min.x = p1.x - r;
        max.x = p0.x + r;
    }
    if (p0.y < p1.y) {
        min.y = p0.y - r;
        max.y = p1.y + r;
    } else {
        min.y = p1.y - r;
        max.y = p0.y + r;
    }
    const double a = p1.x - p0.x;
    const double b = p1.y - p0.y;
    const double closest_dist = fabs(a * (p0.y - p.y) - b * (p0.x - p.x)) / sqrt(a * a + b * b);
    return p < max && min < p && closest_dist < r;
}

bool Chip::collidesOnTranslation(const vec2<double> &q, const vec2<double> &to) {
    const vec2<double> a = this->p;
    if (q.dist(a) < 2.0 * r || to.dist(q) < 2.0 * r)
        return true;

    const vec2<double> vaq = q - a;
    const vec2<double> vab = to - a;
    const double ab_len = vab.norm();
    const double dot = vaq.dot(vab) / ab_len;
    if (dot <= 0 || dot >= ab_len)
        return false;

    // TODO: Check whether dot / ab_len (twice) is needed
    const vec2<double> projection = a + vab * (dot / ab_len);
    return projection.dist(q) < 2.0 * r;
}

double Chip::timeUntilPass(const Chip *c0, const Chip *c1) const {
    double a = c1->p.x - c0->p.x;
    double b = c0->p.y - p.y;
    double c = c0->v.y - v.y;
    double d = c1->v.x - c0->v.x;
    double e = c1->p.y - c0->p.y;
    double f = c0->p.x - p.x;
    double g = c0->v.x - v.x;
    double h = c1->v.y - c0->v.y;

    double x = a * c + b * d - e * g - f * h;
    double y = 2 * (c * d - g * h);

    if (std::fabs(x) < 1e-9 || x * x - 2 * (a * b - e * f) * y < 0.0)
        return 1e100;

    double t;
    if (std::fabs(y) < 1e-9) {
        t = (e * f - a * b) / x;
    } else if (x < 0.0) {
        t = (-sqrt(x * x - 2 * (a * b - e * f) * y) - x) / y;
    } else {
        t = (sqrt(x * x - 2 * (a * b - e * f) * y) - x) / y;
    }

    if (t < -1e-5 || 1.0 < t)
        return 1e100;

    vec2<double> min, max, s = p + v * t, cp0 = c0->p + c0->v * t, cp1 = c1->p + c1->v * t;

    if (cp0.x < cp1.x) {
        min.x = cp0.x;
        max.x = cp1.x;
    } else {
        min.x = cp1.x;
        max.x = cp0.x;
    }

    if (cp0.y < cp1.y) {
        min.y = cp0.y;
        max.y = cp1.y;
    } else {
        min.y = cp1.y;
        max.y = cp0.y;
    }

    if (min.x <= s.x && min.y <= s.y && s.x <= max.x && s.y <= max.y)
        return t;

    return 1e100;
}

double Chip::timeToCollision(const disc &cue) const {
    double d = cue.v.norm();
    if (d <= 1e-6)
        return 1e100;

    vec2<double> points_d = cue.p - p;
    if (points_d.norm() <= r + cue.r + d) {
        double a = cue.v.normSquared();
        double b = cue.v.dot(points_d);
        double c = points_d.normSquared() - (r + cue.r) * (r + cue.r);
        double det = b * b - a * c;
        if (det < 0.0)
            return 1e100;

        double t = (-b - std::sqrt(det)) / a;
        if (t < 0.0 && t + 1e-5 >= -1.0)
            return t;
    }
    return 1e100;
}

// This method assumes that the two chips do not OVERLAP!
double Chip::timeToCollision(const Chip *chip) const {
    double t = 1.0;

    if ((p - chip->p).dot(chip->v - v) > 1e-6) {
        vec2<double> pj = chip->p, vj = chip->v;

        const double a = v.x * v.x + v.y * v.y - 2.0 * v.x * vj.x + vj.x * vj.x - 2.0 * v.y * vj.y + vj.y * vj.y;
        const double b = -p.x * v.x - p.y * v.y + v.x * pj.x + v.y * pj.y + p.x * vj.x - pj.x * vj.x + p.y * vj.y - pj.y * vj.y;
        const double c = p.x * p.x + p.y * p.y - r * r - 2.0 * p.x * pj.x + pj.x * pj.x - 2.0 * p.y * pj.y + pj.y * pj.y - 2.0 * r * chip->r - chip->r * chip->r;
        double s = 4.0 * (b * b - a * c);

        if (s > 0.0) {
            s = 0.5 * sqrt(s);
            // This t is always bigger than 0, except for the case when the two chips overlap.
            t = fabs(std::min(1.0, std::min(std::min(t, (b - s) / a), (b + s) / a)));
        }
    }
    return t;
}

double Chip::timeToCollision(const rect<double> &r) const {
    double t = 1.0;
    if (v.x < -1e-6) {
        t = std::min(t, std::max(p.x - this->r - r.origin.x, 0.0) / -v.x);
    } else if (v.x > 1e-6) {
        t = std::min(t, std::max(r.origin.x + r.size.x - p.x - this->r, 0.0) / v.x);
    }
    if (v.y < -1e-6) {
        t = std::min(t, std::max(p.y - this->r - r.origin.y, 0.0) / -v.y);
    } else if (v.y > 1e-6) {
        t = std::min(t, std::max(r.origin.y + r.size.y - p.y - this->r, 0.0) / v.y);
    }
    return t;
}
