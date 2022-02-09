// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IVIEW_HPP
#define IVIEW_HPP

#include <vector>

#include "button_type.h"
#include "Label.hpp"
#include "Line.hpp"
#include "view_type.hpp"

template<typename T>
class IViewDelegate {
public:
    void tapped(const vec2<float> &p) {
        tapped(view, p);
    }

protected:
    IViewDelegate() : view(new T(this)) {
    }

    ~IViewDelegate() {
        delete view;
    }

    virtual void tapped(T *view, const vec2<float> &p) = 0;

    T *view;
};

class IView {
public:
    // progress [-1,1], -1: before showing, 0: fully showing, 1: after hiding
    virtual void setTransitionProgress(float progress) = 0;

    virtual void tapped(const vec2<float> &p) = 0;

    virtual view_type type() const {
        return view_type::full_screen;
    }

    const std::vector<Label> &labels() const {
        return _labels;
    }

    const std::vector<Line> &lines() const {
        return _lines;
    }

    const std::vector<Circle> &circles() const {
        return _circles;
    }

    const vec4<float> &blurData() const {
        return _blur_data;
    }

    const vec3<float> &zoomData() const {
        return _zoom_data;
    }

protected:
    std::vector<Label>  _labels;
    std::vector<Line>   _lines;
    std::vector<Circle> _circles;
    vec4<float>         _blur_data; // (x,y) is position, z intensity [0,1], w brightness [0,1]
    vec3<float>         _zoom_data {
        assets::screen_size.x / 2.f,   // zoom center x coordinate
        assets::screen_size.y / 2.f,   // zoom center y coordinate
        assets::screen_size.y / 2.f,   // height-percentage of the zoomed in view
    };
};

template<typename T>
class TView : public IView {
public:
    TView(IViewDelegate<T> *delegate, unsigned short label_count = 0, unsigned short line_count = 0, unsigned short circle_count = 0) : _delegate(delegate) {
        if (label_count) {
            _labels.resize(label_count);
        }
        if (line_count) {
            _lines.resize(line_count);
        }
        if (circle_count) {
            _circles.resize(circle_count);
        }
    }

    void tapped(const vec2<float> &p) override {
        _delegate->tapped(p);
    }

private:
    IViewDelegate<T> *_delegate;
};

#endif
