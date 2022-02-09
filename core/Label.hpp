// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef LABEL_HPP
#define LABEL_HPP

#include <string>

#include "assets.hpp"
#include "in_app_purchase_type.h"
#include "Logger.hpp"
#include "rect.hpp"

enum class alignment_type : unsigned char {
    left,
    right,
    center
};

enum class fill_type : unsigned char {
    regular,
    inverted,
    rounded
};

enum font_weight {
    thin,
    regular,
    thick,
    range
};

class Label {
    class Glyph : public rect<float> {
    public:
        Glyph(const vec2<float> &origin, const vec2<float> &size, const rect<float> *uv, font_weight weight, wchar_t c) : rect<float>(origin, size), uv(uv), weight(weight), c(c) {
        }

        const rect<float> *uv = nullptr;
        font_weight        weight = font_weight::thin;
        wchar_t            c = 0;
    };

public:
    Label() = default;

    Label(const vec2<float> &p, float font_height, wchar_t c, font_weight weight, fill_type t = fill_type::regular);

    Label(float font_height, float max_width, fill_type t, const std::wstring &s, alignment_type alignment, font_weight weight = font_weight::regular);

    void setText(const vec2<float> &p, float font_height, wchar_t c, font_weight weight);

    void setText(float font_height, float max_width, const std::wstring &s, alignment_type alignment, font_weight weight = font_weight::regular);

    void clearText();

    void setPosition(const vec2<float> &p);

    bool tapped(const vec2<float> &p) const;

    bool isBackButton() const;

    std::vector<Glyph> text;
    rect<float>        bounds;
    vec2<float>        p;
    vec2<float>        size;
    fill_type          type = fill_type::regular;
    float              alpha = 0.f;
    float              pointer = 0.f;
};

#endif
