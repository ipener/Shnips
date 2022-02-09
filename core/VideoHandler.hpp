// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef VIDEOHANDLER_HPP
#define VIDEOHANDLER_HPP

#include "IGameHandler.hpp"
#include "replay_state.hpp"
#include "video_type.h"

class VideoHandler : public IGameHandler {
public:
    VideoHandler(::base_state *base_state);

    void init(class IVideoSceneDelegate *scene_delegate);

    void hit(unsigned short i) override;

    void update(float t) override;

    void onChipsRestored() override;

    void willBecomeActive() override;

    void willResignActive() override;

    bool collisionEnabled() const override;

    bool interactionAllowed() const override;

    void setVideoType(video_type type);

    void setTransitionProgress(float progress);

    void startExitTransition();

private:
    const float                  _video_duration = 3.f;
    const unsigned short         _video_last_index = 1;

    class IVideoHandlerDelegate *_delegate = nullptr;
    class IVideoSceneDelegate   *_scene_delegate = nullptr;

    std::vector<Chip>            _initial_chips_data;
    replay_state                 _video_states[video_type_range];
    disc                         _indicator_cue;
    rect<double>                 _initial_bounds;
    rect<double>                 _view_bounds;
    video_type                   _current_video = video_type_no_cushion_shot;

    unsigned short               _initial_last_index;
    float                        _slide_x_offset = 0.f;
    bool                         _video_playing = false;
    bool                         _exit_transition_running = false;
};

#endif
