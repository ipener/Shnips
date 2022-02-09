// Copyright © 2018 Igor Pener. All rights reserved.

#include "Logger.hpp"

#ifndef __APPLE__
#include <android/log.h>
#endif

template<log_level L> Logger<L>::ErrorLogger::~ErrorLogger() {
    pubsync();
}

template<log_level L> int Logger<L>::ErrorLogger::sync() {
#ifdef DEBUG
    auto string_type = str();

    if (string_type.empty())
        return 0;

#ifdef __APPLE__
    std::cerr << string_type;
#else
    __android_log_print(ANDROID_LOG_ERROR, "libgl2jni", "%s", string_type.c_str());
#endif
#endif
    str("");

    return 0;
}

template<log_level L> Logger<L>::InfoLogger::~InfoLogger() {
    pubsync();
}

template<log_level L> int Logger<L>::InfoLogger::sync() {
#ifdef DEBUG
    auto string_type = str();

    if (string_type.empty())
        return 0;

#ifdef __APPLE__
    std::cout << string_type;
#else
    __android_log_print(ANDROID_LOG_INFO, "libgl2jni", "%s", string_type.c_str());
#endif
#endif
    str("");

    return 0;
}

template<> Logger<log_level::error>::Logger() : std::ostream(new Logger::ErrorLogger()) {}

template<> Logger<log_level::info>::Logger() : std::ostream(new Logger::InfoLogger()) {}

template<log_level L> Logger<L>::Logger::~Logger() {
    delete rdbuf();
}

Logger<log_level::error> loge;
Logger<log_level::info> logi;
