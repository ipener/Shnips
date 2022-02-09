// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef TUTORIALHANDLER_HPP
#define TUTORIALHANDLER_HPP

#include "app_state.hpp"
#include "IGameHandler.hpp"
#include "replay_state.hpp"
#include "tutorial_type.h"

class TutorialHandler : public GameStateHandler<app_state::tutorial> {
public:
    TutorialHandler(::base_state *base_state, IStateHandler *state_handler);

    void init(class ITutorialHandlerDelegate *delegate, class ITutorialSceneDelegate *scene_delegate);

    void passed(unsigned short, unsigned short) override;

    void hit(unsigned short i) override;

    void cushionHit() override;

    void submit() override;

    void update(float t) override;

    void touchesBegan(const vec2<double> &p, long long t_ms) override;

    void touchesEnded(const vec2<double> &p, long long t_ms) override;

    void onChipsRestored() override;

    void willBecomeActive() override;

    bool collisionEnabled() const override;

    bool playableGame() const override;

    void showNextTutorial();

    void cancelTutorial();

private:
    void resetCue();

    void changePage();

    void setShotStates();

    void maybeShowCue();

    bool shouldShowCue() const;

private:
    class ITutorialHandlerDelegate *_delegate = nullptr;
    class ITutorialSceneDelegate   *_scene_delegate = nullptr;

    tutorial_type                  _current_tutorial = tutorial_type_simple_shot;
    replay_state                   _shot_states[tutorial_type_range];
    disc                           _indicator_cue;
    vec2<double>                   _first_shot_velocity;
    vec2<double>                   _first_shot_pass_velocity;

    unsigned long                  _score = 0;
    unsigned short                 _shot_score = 0;
    float                          _notification_animation_progress = 0.f;
    bool                           _touching_field = false;
    bool                           _cue_animation_running = false;
    bool                           _shot_succeeded = false;
    bool                           _consecutive_shot_failed = false;
    bool                           _first_shot_speed_adjusted = false;
    bool                           _notification_animation_running = false;
};

#endif
