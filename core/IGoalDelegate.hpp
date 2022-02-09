// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IGOALDELEGATE_HPP
#define IGOALDELEGATE_HPP

class IGoalDelegate {
public:
    virtual void goalCompleted() = 0;

    virtual void setProgress(unsigned long long progress) = 0;
};

#endif
