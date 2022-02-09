// Copyright © 2018 Igor Pener. All rights reserved.

#include "Label.hpp"

using namespace std;

Label::Label(const vec2<float> &p, float font_height, wchar_t c, font_weight weight, fill_type t) : type(t) {
    setText(p, font_height, c, weight);
}

Label::Label(float font_height, float max_width, fill_type t, const wstring &s, alignment_type alignment, font_weight weight) : type(t) {
    if (s.length()) {
        setText(font_height, max_width, s, alignment, weight);
    }
}

void Label::setText(const vec2<float> &p, float font_height, wchar_t c, font_weight weight) {
    text.clear();

    this->p = p;
    const rect<float> *uv;
    const auto it = assets::texture_atlas.find(c);

    if (it != assets::texture_atlas.end()) {
        uv = &it->second.uv[weight];
    } else {
        uv = &assets::texture_atlas.find(L'.')->second.uv[weight];
        logi << "Unexpected character requested: " << c << endl;
    }
    size.x = uv->size.x * font_height / uv->size.y;
    size.y = font_height;
    text.push_back({p - size / 2.f, size, uv, weight, c});
}

void Label::setText(float font_height, float max_width, const wstring &s, alignment_type alignment, font_weight weight) {
    text.clear();

    const auto length = s.length();
    size = {max_width, font_height};
    vec2<float> dp {0.f, font_height};
    const rect<float> *uv;
    float w;

    if (font_height < 1e-5f || max_width < 1e-5f || !length)
        return;

    vector<size_t> new_line_indecies {0};
    unordered_map<wchar_t, tex_coords>::const_iterator it;
    font_weight current_weigth = weight;
    bool marker_found = false;

    for (size_t i = 0, j = 0, k = 0; j < length; ++j) {
        for (; j < length; ++j) {
            if (s[j] == L' ' || s[j] == L'\n' || s[j] == L'\u00A0')
                break;

            if (s[j] == L'*') {
                if (marker_found) {
                    marker_found = false;
                    current_weigth = weight;
                } else {
                    marker_found = true;
                    current_weigth = font_weight::thick;
                }
                break;
            }
            it = assets::texture_atlas.find(s[j]);
            if (it != assets::texture_atlas.end()) {
                uv = &it->second.uv[current_weigth];
            } else {
                uv = &assets::texture_atlas.find(L'.')->second.uv[current_weigth];
                logi << "Unexpected character requested: " << s[j] << endl;
            }

            w = uv->size.x * font_height / uv->size.y;
            text.push_back({dp, {w, font_height}, uv, current_weigth, s[j]});
            dp.x += w;
        }
        if (dp.x < max_width) {
            k = i != 0 ? k + (j - i - 1) : j;
            i = j;
        } else {
            size.y += font_height;
            dp.x = 0.f;
            dp.y += font_height;
            new_line_indecies.push_back(k);

            for (; ++i < j; ++k) {
                uv = text[k].uv;
                w = uv->size.x * font_height / uv->size.y;
                text[k].origin = dp;
                text[k].size.x = w;
                dp.x += w;
            }
        }
        if (s[j] == L'\n') {
            size.y += font_height;
            dp.x = 0.f;
            dp.y += font_height;
            new_line_indecies.push_back(k);
        } else if (s[j] != L'*') {
            dp.x += font_height * assets::ui_empty_space_multiplier;
        }
    }

    if (alignment == alignment_type::center) {
        for (size_t j, i = 0; i < new_line_indecies.size(); ++i) {
            j = i + 1 < new_line_indecies.size() ? new_line_indecies[i + 1] - 1 : text.size() - 1;
            w = text[j].origin.x + text[j].size.x - text[new_line_indecies[i]].origin.x;
            w = (max_width - w) / 2.f;

            for (size_t k = new_line_indecies[i]; k <= j; ++k) {
                text[k].origin.x += w;
                text[k].origin.y = size.y - text[k].origin.y;
            }
        }
    } else if (alignment == alignment_type::right) {
        for (size_t j, i = 0; i < new_line_indecies.size(); ++i) {
            j = i + 1 < new_line_indecies.size() ? new_line_indecies[i + 1] - 1 : text.size() - 1;
            w = text[j].origin.x + text[j].size.x - text[new_line_indecies[i]].origin.x;
            w = max_width - w;

            for (size_t k = new_line_indecies[i]; k <= j; ++k) {
                text[k].origin.x += w;
                text[k].origin.y = size.y - text[k].origin.y;
            }
        }
    } else {
        if (fabs(size.y - font_height) < 1e-5f) {
            size.x = text.back().origin.x - text.front().origin.x + text.back().size.x;
        }
        for (auto &c : text) {
            c.origin.y = size.y - c.origin.y;
        }
    }
    this->p = size / 2.f;
}

void Label::clearText() {
    text.clear();
    size = p ={};
}

void Label::setPosition(const vec2<float> &p) {
    const auto dp = p - this->p;
    if (fabs(dp.x) > 1e-5f || fabs(dp.y) > 1e-5f) {
        this->p = p;
        bounds.origin += dp;
        for (auto &c : text) {
            c.origin += dp;
        }
    }
}

bool Label::tapped(const vec2<float> &p) const {
    return alpha > 0.99999f &&
    fabs(p.x - this->p.x) < (size.x + assets::ui_margin) / 2.f &&
    fabs(p.y - this->p.y) < (size.y + assets::ui_margin) / 2.f;
}

bool Label::isBackButton() const {
    return text.size() == 1 && text[0].c == assets::glyphs::buttons[button_type_cross];
}
