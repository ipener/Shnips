// Copyright © 2018 Igor Pener. All rights reserved.

#include "IStateHandler.hpp"
#include "json.hpp"

void IStateHandler::save(const app_state::singleplayer &state) {
    const auto encoded = json::encode(state);
    save(encoded, json::key(app_state::key_singleplayer));
}

void IStateHandler::save(const app_state::store &state) {
    const auto encoded = json::encode(state);
    save(encoded, json::key(app_state::key_store));
}

void IStateHandler::save(const app_state::tutorial &state) {
    const auto encoded = json::encode(state);
    save(encoded, json::key(app_state::key_tutorial));
}

void IStateHandler::save(const app_state::achievements &state) {
    const auto encoded = json::encode(state);
    save(encoded, json::key(app_state::key_achievements));
}

void IStateHandler::save(const app_state &state) {
    const auto encoded = json::encode(state);
    save(encoded, json::key(key_app_state));
}

bool IStateHandler::load(app_state::singleplayer *state) {
    std::string data;
    return load(data, json::key(app_state::key_singleplayer)) ? json::decode(*state, data.c_str()) : false;
}

bool IStateHandler::load(app_state::store *state) {
    std::string data;
    return load(data, json::key(app_state::key_store)) ? json::decode(*state, data.c_str()) : false;
}

bool IStateHandler::load(app_state::tutorial *state) {
    std::string data;
    return load(data, json::key(app_state::key_tutorial)) ? json::decode(*state, data.c_str()) : false;
}

bool IStateHandler::load(app_state::achievements *state) {
    std::string data;
    return load(data, json::key(app_state::key_achievements)) ? json::decode(*state, data.c_str()) : false;
}

bool IStateHandler::load(app_state *state) {
    std::string data;
    return load(data, json::key(key_app_state)) ? json::decode(*state, data.c_str()) : false;
}
