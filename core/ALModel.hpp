// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ALMODEL_HPP
#define ALMODEL_HPP

#ifdef __APPLE__
#include <OpenAl/al.h>
#include <OpenAl/alc.h>
#else
#include "AL/al.h"
#include "AL/alc.h"
#endif

struct ALModel {
    ALuint buffer;
    ALuint sources;
    float  gain;
};

#endif
