// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef ALRENDERER_HPP
#define ALRENDERER_HPP

#include "ALModel.hpp"
#include "sound_type.hpp"

// Convert audio files by running: afconvert -f caff -d LEI16@44100 <file>.mp3 <file>.caf
class ALRenderer {
public:
    ALRenderer(class IResourceLoader *resource_loader);

    ~ALRenderer();

    void restoreContext();

    void freeContext();

    template<typename T> void playSound(T type);
    
    template<sound_type T> void playSound();

    template<sound_type T> void stopSound();

    template<sound_type T> void setVolume(float volume);

    void pause();

    void resume();

    void mute();

    void unmute();

private:
    void clear();

    void alDeleteModel(ALModel *model, unsigned short count = 1);

    void alSetupModel(class IResourceLoader *resource_loader, ALModel *model, const char *filename, unsigned short count = 1);

    void pause(ALModel *model);

    void resume(ALModel *model);

private:
    // To be safe on 'most' devices we should use not more than 32 sound buffers
    static const unsigned short _chip_passed_buffer_count = 8;
    static const unsigned short _chip_charged_buffer_count = 3;
    static const unsigned short _cushion_hit_buffer_count = 8;
    static const unsigned short _chip_hit_buffer_count = 4;

    ALCdevice                   *_device = nullptr;
    ALCcontext                  *_context = nullptr;
    
    ALModel                     _skill_shot_achieved_model[skill_shot_type_range];
    ALModel                     _achievement_unlocked_model;
    ALModel                     _goal_completed_model;
    ALModel                     _chip_passed_model[_chip_passed_buffer_count];
    ALModel                     _chip_charged_model[_chip_charged_buffer_count];
    ALModel                     _chip_hit_model[_chip_hit_buffer_count];
    ALModel                     _cushion_hit_model[_cushion_hit_buffer_count];
    ALModel                     _cue_hit_model;
    ALModel                     _sliding_model;
    ALModel                     _missed_model;

    unsigned short              _current_chip_passed_index = 0;
    unsigned short              _current_chip_charged_index = 0;
    unsigned short              _current_cushion_hit_index = 0;
    unsigned short              _current_chip_hit_index = 0;
    bool                        _muted = false;
};

#endif
