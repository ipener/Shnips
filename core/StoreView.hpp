// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef STOREVIEW_HPP
#define STOREVIEW_HPP

#include "chip_type.h"
#include "IView.hpp"

class StoreView : public TView<StoreView> {
public:
    StoreView(IViewDelegate<StoreView> *delegate);

    void setTransitionProgress(float progress) override;

    template<button_type>
    bool tapped(const vec2<float> &p) const;

    void setViewAlpha(float alpha);

    void setChipType(chip_type type, unsigned short level, float progress);

    void setGoalsProgress(unsigned short completed, unsigned short total);

    void setIndicatorLevel(unsigned short index, unsigned short level);

    void setIndicatorAlpha(unsigned short index, float alpha);

    void setBlurIntensity(float blur_intensity);

private:
    vec3<float> _zoomed_in_data;
    chip_type   _chip_type = chip_type_default;
    float       _chip_progress = 0.f;
    bool        _chip_selectable = true;

};

#endif
