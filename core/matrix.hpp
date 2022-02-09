// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cmath>

struct mat3 {
    float m[9];

    inline mat3 invertAndTranspose() {
        const float det = m[0] * (m[4] * m[8] - m[5] * m[7]) - m[3] * (m[1] * m[8] - m[7] * m[2]) + m[6] * (m[1] * m[5] - m[4] * m[2]);
        const float invdet = 1.f / det;
        return {
            (m[4] * m[8] - m[5] * m[7]) * invdet,
            (m[6] * m[5] - m[3] * m[8]) * invdet,
            (m[3] * m[7] - m[6] * m[4]) * invdet,
            (m[7] * m[2] - m[1] * m[8]) * invdet,
            (m[0] * m[8] - m[6] * m[2]) * invdet,
            (m[1] * m[6] - m[0] * m[7]) * invdet,
            (m[1] * m[5] - m[2] * m[4]) * invdet,
            (m[2] * m[3] - m[0] * m[5]) * invdet,
            (m[0] * m[4] - m[1] * m[3]) * invdet
        };
    }
};

struct mat4 {
    float m[16];

    static inline mat4 perspective(float fovy_radians, float aspect, float near_z, float far_z) {
        const float cotan = 1.f / tanf(fovy_radians * 0.5f);
        return {
            cotan / aspect, 0.f, 0.f, 0.f,
            0.f, cotan, 0.f, 0.f,
            0.f, 0.f, (far_z + near_z) / (near_z - far_z), -1.f,
            0.f, 0.f, (2.f * far_z * near_z) / (near_z - far_z), 0.f
        };

    }

    static inline mat4 translation(float x, float y, float z) {
        return {
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
              x,   y,   z, 1.f
        };
    }

    static inline mat4 scaleMat(float x, float y, float z) {
        return {
              x, 0.f, 0.f, 0.f,
            0.f,   y, 0.f, 0.f,
            0.f, 0.f,   z, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
    }

    mat4 operator * (const mat4 &a) const {
        return {
            m[0] * a.m[0] + m[4] * a.m[1] + m[8] * a.m[2] + m[12] * a.m[3],
            m[1] * a.m[0] + m[5] * a.m[1] + m[9] * a.m[2] + m[13] * a.m[3],
            m[2] * a.m[0] + m[6] * a.m[1] + m[10] * a.m[2] + m[14] * a.m[3],
            m[3] * a.m[0] + m[7] * a.m[1] + m[11] * a.m[2] + m[15] * a.m[3],
            m[0] * a.m[4] + m[4] * a.m[5] + m[8] * a.m[6] + m[12] * a.m[7],
            m[1] * a.m[4] + m[5] * a.m[5] + m[9] * a.m[6] + m[13] * a.m[7],
            m[2] * a.m[4] + m[6] * a.m[5] + m[10] * a.m[6] + m[14] * a.m[7],
            m[3] * a.m[4] + m[7] * a.m[5] + m[11] * a.m[6] + m[15] * a.m[7],
            m[0] * a.m[8] + m[4] * a.m[9] + m[8] * a.m[10] + m[12] * a.m[11],
            m[1] * a.m[8] + m[5] * a.m[9] + m[9] * a.m[10] + m[13] * a.m[11],
            m[2] * a.m[8] + m[6] * a.m[9] + m[10] * a.m[10] + m[14] * a.m[11],
            m[3] * a.m[8] + m[7] * a.m[9] + m[11] * a.m[10] + m[15] * a.m[11],
            m[0] * a.m[12] + m[4] * a.m[13] + m[8] * a.m[14] + m[12] * a.m[15],
            m[1] * a.m[12] + m[5] * a.m[13] + m[9] * a.m[14] + m[13] * a.m[15],
            m[2] * a.m[12] + m[6] * a.m[13] + m[10] * a.m[14] + m[14] * a.m[15],
            m[3] * a.m[12] + m[7] * a.m[13] + m[11] * a.m[14] + m[15] * a.m[15]
        };
    }

    inline mat4 translate(float x, float y, float z) const {
        return {
            m[0] + m[3] * x, m[1] + m[3] * y, m[2] + m[3] * z, m[3],
            m[4] + m[7] * x, m[5] + m[7] * y, m[6] + m[7] * z, m[7],
            m[8] + m[11] * x, m[9] + m[11] * y, m[10] + m[11] * z, m[11],
            m[12] + m[15] * x, m[13] + m[15] * y, m[14] + m[15] * z, m[15]
        };
    }

    inline mat4 scale(float x, float y, float z) const {
        return {
            m[0] * x, m[1] * y, m[2] * z, m[3],
            m[4] * x, m[5] * y, m[6] * z, m[7],
            m[8] * x, m[9] * y, m[10] * z, m[11],
            m[12] * x, m[13] * y, m[14] * z, m[15]
        };
    }

    inline mat4 rotateZ(float rad) const {
        const float cos_rad = cosf(rad);
        const float sin_rad = sinf(rad);
        return {
            m[0] * cos_rad - m[1] * sin_rad, m[0] * sin_rad + m[1] * cos_rad, m[2], m[3],
            m[4] * cos_rad - m[5] * sin_rad, m[4] * sin_rad + m[5] * cos_rad, m[6], m[7],
            m[8] * cos_rad - m[9] * sin_rad, m[8] * sin_rad + m[9] * cos_rad, m[10], m[11],
            m[12] * cos_rad - m[13] * sin_rad, m[12] * sin_rad + m[13] * cos_rad, m[14], m[15]
        };
    }

    inline mat3 getMat3() const {
        return {m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]};
    }
};

#endif
