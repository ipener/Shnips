// Copyright © 2018 Igor Pener. All rights reserved.

#include "utils.hpp"

#include <chrono>

using namespace std::chrono;

namespace utils {
    long long currentTimestampMs() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    unsigned short countBits(unsigned long long n) {
        unsigned short ans = 0;
        for (ans = 0; n > 0; ++ans) {
            n -= n & (-n);
        }
        return ans;
    }
}
