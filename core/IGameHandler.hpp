// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef IGAMEHANDLER_HPP
#define IGAMEHANDLER_HPP

#include "app_state.hpp"
#include "assets.hpp"
#include "IStateHandler.hpp"

class IGameHandler {
public:
    IGameHandler(::base_state *base_state) : _base_state(base_state) {
    }

    virtual ~IGameHandler() {
        _base_state = nullptr;
    }

    virtual void passed(unsigned short i, unsigned short j) {
    }

    // i == 0xFFFF means the chip _base_state->chips_data[_base_state->last_index] was hit
    virtual void hit(unsigned short i) {
        _hit = true;
    }

    virtual void chipHit(unsigned short i) {
    }

    virtual void cushionHit() {
    }

    virtual void submit() {
        _hit = false;
    }

    virtual void update(float t) {}

    virtual void touchesBegan(const vec2<double> &p, long long t_ms) {
    }

    virtual void touchesMoved(const vec2<double> &p, long long t_ms) {
    }

    virtual void touchesEnded(const vec2<double> &p, long long t_ms) {
    }

    virtual void onChipsRestored() {
    }

    virtual void willBecomeActive() {
    }

    virtual void willResignActive() {
    }

    virtual bool hit() const {
        return _hit;
    }

    virtual bool collisionEnabled() const {
        return true;
    }

    virtual bool interactionAllowed() const {
        return true;
    }

    virtual bool playableGame() const {
        return false;
    }

protected:
    base_state *_base_state = nullptr;
    bool        _hit = false;
};

template<typename T>
class GameStateHandler : public IGameHandler {
public:
    GameStateHandler(::base_state *base_state, IStateHandler *state_handler) : IGameHandler(base_state), _state_handler(state_handler) {
    }

    const T &state() const {
        return _state;
    }

    void save() {
        _state_handler->save(_state);
    }

protected:
    bool load() {
        return _state_handler->load(&_state);
    }

protected:
    T _state;

private:
    IStateHandler *_state_handler = nullptr;
};

#endif
