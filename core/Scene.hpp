// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef SCENE_HPP
#define SCENE_HPP

#include "base_state.hpp"
#include "ISingleplayerSceneDelegate.hpp"
#include "ITutorialSceneDelegate.hpp"
#include "IVideoSceneDelegate.hpp"
#include "Label.hpp"
#include "sound_type.hpp"

class Scene : public ISingleplayerSceneDelegate, public ITutorialSceneDelegate, public IVideoSceneDelegate {
public:
    Scene(class IRenderer *renderer, class ALRenderer *al_renderer, base_state *state);

    void init();

    void update(float t);
    
    void render();

    void touchesBegan(const vec2<double> &p, long long t_ms);

    void touchesMoved(const vec2<double> &p, long long t_ms);

    void touchesEnded(const vec2<double> &p, long long t_ms);

    void setChargeAlpha(float alpha);

    void setHeartAlpha(unsigned short index, float alpha);

    void setGameHandler(class IGameHandler *handler);

    void setFullScreenView(const class IView *view);

    void setAlertView(const class IView *view);

    void setBackgroundBlending(bool enabled);

    bool chipsMoving() const;

private:
    struct blend_quad {
        bool         white = false;
        vec2<float>  points[4];
        float        alphas[4];
    };

    struct blend_spot {
        bool        white;
        float       alpha;
        vec2<float> p;
    };

    // MARK: - IVideoHandlerSceneDelegate
    void collideWithCue() override;

    // MARK: - ISingleplayerSceneDelegate
    void addExtraChips(unsigned short n) override;

    void removeExtraChips() override;

    void restoreChips(std::vector<vec2<double>> positions, unsigned short last_index = 0xFFFF, bool clear_background = true) override;

    void setChipsSpeedCoefficient(float speed_coeff) override;

    void setChargedAnimation(bool start, bool play_sound) override;

    void setChargeProgress(float progress) override;

    bool willPass(base_state &state) const override;

    void skillShotAchieved(skill_shot_type type) override;

    // MARK: -
    void removeOneBlankChip();

    void collideWithCue(double frame_speed_coefficient, bool ignore_chip_speed_coeff);
    
    void collide(float time_difference);

    double timeUntilCollision(const base_state &state) const;

    void updateScene(base_state &state, double t, unsigned short &chips_moving) const;

    void passed(unsigned short i, unsigned short j);

    void updateSlidingVolume();

    void computeBlendQuadVertices(vec2<float> points[4], const vec2<double> &p, const vec2<double> &q);

    double getFrameSpeedCoefficient(long long t);

    bool restoreChipsAnimationRunning() const;

private:
    struct transition_data {
        vec2<double> initial_p, target_p;
    };

    const class IView           *_full_screen_view = nullptr;
    const class IView           *_alert_view = nullptr;
    const float                  _halo_animation_step = 0.75f;
    const float                  _minimum_background_blend_step = 1.f / 255.f;
    const float                  _default_background_blend_step = 0.0001f;
    const float                  _default_background_blend_clear_step = 0.02f;
    const float                  _restore_animation_transition_step = 8.f;

    class ALRenderer            *_al_renderer = nullptr;
    class IRenderer             *_gl_renderer = nullptr;
    class IGameHandler          *_handler = nullptr;
    base_state                  *_state = nullptr;

    std::vector<Label>           _heart_labels {3};
    std::vector<Chip>            _extra_chips_to_be_removed;
    std::vector<transition_data> _extra_chips_transition_data;
    transition_data              _restore_animation_data[assets::default_chips_count];

    blend_quad                   _blend_quad;
    blend_spot                   _blend_cushion_hit {false, 0.3f};
    blend_spot                   _blend_hit {true, 0.4f};
    vec2<double>                 _last_touch_location;
    vec2<double>                 _touch_velocity;

    long long                    _last_touch_t;
    double                       _cross_product[8][8];
    float                        _halo_animation_progress = 0.f;
    float                        _chips_speed_coefficient = 1.f;
    float                        _max_velocity = 40.f;
    float                        _charge_alpha = 0.f;
    float                        _charge_progress = 0.f;
    float                        _restore_animation_progress = 0.f;
    float                        _background_blend_step = 0.f;
    float                        _background_blend_clear_animation_progress = 1.f;
    float                        _extra_chips_animation_progress = 0.f;
    unsigned short               _extra_chips_index = 0;
    unsigned short               _restore_animation_index = assets::default_chips_count;
    unsigned short               _chips_moving = 0;
    bool                         _should_pass[8][8];
    bool                         _pass_signaled[8][8];
    bool                         _invalid_touch = false;
    bool                         _playing_sound = false;
    bool                         _charged_animation_running = false;
    bool                         _hit_blank = false;
    bool                         _blending_enabled = true;
    bool                         _blend_quad_available = false;
};

#endif
