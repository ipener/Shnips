// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ITUTORIALHANDLERDELEGATE_HPP
#define ITUTORIALHANDLERDELEGATE_HPP

#include "tutorial_type.h"

class ITutorialHandlerDelegate {
public:
    virtual void setCancelable(bool cancelable) = 0;

    virtual void setTutorialScore(unsigned long long score) = 0;

    virtual void setTutorialShotScore(unsigned long long score) = 0;

    virtual void setTutorialTitle(tutorial_type type) = 0;

    virtual void setTutorialTitleAlpha(float alpha) = 0;

    virtual void exitTutorial() = 0;

    virtual void showAlert(tutorial_type type) = 0;
};

#endif
