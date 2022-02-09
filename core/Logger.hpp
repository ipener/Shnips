// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <sstream>

enum class log_level : unsigned char {
    error,
    info
};

template<log_level L>
class Logger : public std::ostream {
    class ErrorLogger : public std::stringbuf {
    public:
        ~ErrorLogger();

        int sync() override;
    };

    class InfoLogger : public std::stringbuf {
    public:
        ~InfoLogger();

        int sync() override;
    };

public:
    Logger();

    ~Logger();
};

extern Logger<log_level::error> loge;
extern Logger<log_level::info> logi;

#endif
