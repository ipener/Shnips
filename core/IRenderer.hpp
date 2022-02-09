// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IRENDERER_H
#define IRENDERER_H

#include "Label.hpp"
#include "Line.hpp"
#include "view_type.hpp"

class IRenderer {
public:
    virtual void prepareFrame() = 0;

    virtual void prepareLineDraw() = 0;

    virtual void prepareCircleDraw() = 0;

    virtual void prepareLabelDraw() = 0;

    virtual void prepareQuadBlending(float delta) = 0;

    virtual void drawLine(const Line &l, float progress) = 0;

    virtual void drawLine(const Line &l) = 0;

    virtual void drawCircle(const Circle &c, float thickness) = 0;

    virtual void drawHalo(const vec2<double> &p, float size, float thickness, const float color[4]) = 0;

    virtual void drawCue(const vec2<double> &p, float alpha) = 0;

    virtual void drawBlendQuad(const vec2<float> points[4], const float alphas[4], bool white) = 0;

    virtual void drawBlendSpot(const vec2<float> &p, bool white, float alpha) = 0;

    virtual void drawShadows(const std::vector<Chip> &chips) = 0;

    virtual void drawChips(const std::vector<Chip> &chips) = 0;

    virtual void drawLabels(const std::vector<Label> &labels) = 0;

    virtual void setBlendEquation(bool additive) = 0;

    virtual void setZoomData(const vec3<float> &data) = 0;

    virtual void setBlurData(view_type type, const vec4<float> &data) = 0;

    virtual void loadGLResources() = 0;
};

#endif
