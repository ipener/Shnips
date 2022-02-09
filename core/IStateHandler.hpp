// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ISTATEHANDLER_HPP
#define ISTATEHANDLER_HPP

#include <string>
#include "app_state.hpp"

class IStateHandler {
public:
    virtual ~IStateHandler() {
    }

    void save(const app_state::singleplayer &state);

    void save(const app_state::store &state);

    void save(const app_state::tutorial &state);

    void save(const app_state::achievements &state);

    void save(const app_state &state);

    bool load(app_state::singleplayer *state);

    bool load(app_state::store *state);

    bool load(app_state::tutorial *state);

    bool load(app_state::achievements *state);

    bool load(app_state *state);

public:
    virtual void reset() = 0;

protected:
    virtual void save(const std::string &data, const std::string &key) = 0;

    virtual bool load(std::string &data, const std::string &key) = 0;
};

#endif
