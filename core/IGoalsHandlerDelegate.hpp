// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IGOALSHANDLERDELEGATE_HPP
#define IGOALSHANDLERDELEGATE_HPP

#include "video_type.h"

class IGoalsHandlerDelegate {
public:
    virtual void goalCompleted(unsigned short id) = 0;

    virtual void setProgress(unsigned long long progress) = 0;

    virtual void setVideoType(video_type type) = 0;
};

#endif
