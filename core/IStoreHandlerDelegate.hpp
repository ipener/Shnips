// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ISTOREHANDLERDELEGATE_HPP
#define ISTOREHANDLERDELEGATE_HPP

#include "chip_type.h"

class IStoreHandlerDelegate {
public:
    virtual void setBackgroundData(const float gradient[6], float blur_intensity) = 0;

    virtual void setChipType(chip_type type, unsigned short level, float progress) = 0;

    virtual void setGoalsProgress(unsigned short total) = 0;

    virtual void setIndicatorLevel(unsigned short index, unsigned short level) = 0;

    virtual void setIndicatorAlpha(unsigned short index, float alpha) = 0;

    virtual void setViewAlpha(float alpha) = 0;

    virtual void didChangePage() = 0;
};

#endif
