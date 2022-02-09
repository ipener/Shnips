// Copyright © 2018 Igor Pener. All rights reserved.

#include "ALRenderer.hpp"

#include <cmath>
#include <cstdlib>
#include <exception>
#include "IResourceLoader.hpp"
#include "Logger.hpp"

#ifdef DEBUG
#define CHECK_AL_ERROR(x)\
{\
ALCenum alc_error = ALC_NO_ERROR;\
while ((alc_error = alcGetError(_device)) != ALC_NO_ERROR) {\
    loge << x << " ALC error occured: " << alc_error << std::endl;\
}\
ALenum al_error = AL_NO_ERROR;\
while ((al_error = alGetError()) != AL_NO_ERROR) {\
    loge << x << " AL error occured: " << al_error << std::endl;\
}\
}
#else
#define CHECK_AL_ERROR(x)
#endif

ALRenderer::ALRenderer(IResourceLoader *resource_loader) {
    _device = alcOpenDevice(0);
    _context = alcCreateContext(_device, 0);
    alcMakeContextCurrent(_context);

    alSetupModel(resource_loader, &_skill_shot_achieved_model[0], "skill_shot_indirect_pass");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[1], "skill_shot_just_passed");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[2], "skill_shot_narrow_gap");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[3], "skill_shot_three_cushion_hit");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[4], "skill_shot_four_passes");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[5], "skill_shot_chips_close");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[6], "skill_shot_two_passes_no_cushion");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[7], "skill_shot_circumvent_chip");
    alSetupModel(resource_loader, &_skill_shot_achieved_model[8], "skill_shot_hit_chips_before_pass");
    alSetupModel(resource_loader, _chip_passed_model, "chip_passed", _chip_passed_buffer_count);
    alSetupModel(resource_loader, _chip_charged_model, "chip_charged", _chip_charged_buffer_count);
    alSetupModel(resource_loader, _chip_hit_model, "chip_hit", _chip_hit_buffer_count);
    alSetupModel(resource_loader, _cushion_hit_model, "cushion_hit", _cushion_hit_buffer_count);
    alSetupModel(resource_loader, &_achievement_unlocked_model, "achievement_unlocked");
    alSetupModel(resource_loader, &_goal_completed_model, "goal_completed");
    alSetupModel(resource_loader, &_cue_hit_model, "cue_hit");
    alSetupModel(resource_loader, &_sliding_model, "sliding");
    alSetupModel(resource_loader, &_missed_model, "missed");

    // TODO: Save all sounds with same relative volume
    for (unsigned short i = 0; i < skill_shot_type_range; ++i) {
        _skill_shot_achieved_model[i].gain = 1.f;
        alSourcef(_skill_shot_achieved_model[i].sources, AL_GAIN, _skill_shot_achieved_model[i].gain);
    }
    for (unsigned short i = 0; i < _chip_passed_buffer_count; ++i) {
        _chip_passed_model[i].gain = 0.01f;
        alSourcef(_chip_passed_model[i].sources, AL_GAIN, _chip_passed_model[i].gain);
    }
    for (unsigned short i = 0; i < _chip_charged_buffer_count; ++i) {
        _chip_charged_model[i].gain = 0.1f;
        alSourcef(_chip_charged_model[i].sources, AL_GAIN, _chip_charged_model[i].gain);
    }
    for (unsigned short i = 0; i < _chip_hit_buffer_count; ++i) {
        _chip_hit_model[i].gain = 0.1f;
        alSourcef(_chip_hit_model[i].sources, AL_GAIN, _chip_hit_model[i].gain);
    }
    for (unsigned short i = 0; i < _cushion_hit_buffer_count; ++i) {
        _cushion_hit_model[i].gain = 1.f;
        alSourcef(_cushion_hit_model[i].sources, AL_GAIN, _cushion_hit_model[i].gain);
    }

    _achievement_unlocked_model.gain = 1.f;
    alSourcef(_achievement_unlocked_model.sources, AL_GAIN, _achievement_unlocked_model.gain);

    _goal_completed_model.gain = 1.f;
    alSourcef(_goal_completed_model.sources, AL_GAIN, _goal_completed_model.gain);

    _cue_hit_model.gain = 0.1f;
    alSourcef(_cue_hit_model.sources, AL_GAIN, _cue_hit_model.gain);

    _missed_model.gain = 1.f;
    alSourcef(_missed_model.sources, AL_GAIN, _missed_model.gain);

    _sliding_model.gain = 0.f;
    alSourcef(_sliding_model.sources, AL_GAIN, _sliding_model.gain);

    CHECK_AL_ERROR(__FUNCTION__)
}

ALRenderer::~ALRenderer() {
    try {
        clear();

        ALCenum alc_error = ALC_NO_ERROR;
        while ((alc_error = alcGetError(_device)) != ALC_NO_ERROR) {
            loge << "ALC error occured: " << alc_error << std::endl;
        }

        alcDestroyContext(_context);
        alcCloseDevice(_device);

        ALenum al_error = AL_NO_ERROR;
        while ((al_error = alGetError()) != AL_NO_ERROR) {
            loge << "AL error occured: " << al_error << std::endl;
        }
    } catch (const std::exception &e) {
        loge << "Exception occured in ~ALRenderer: " << e.what() << std::endl;
    }
}

void ALRenderer::restoreContext() {
    alcMakeContextCurrent(_context);
    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::freeContext() {
    alcMakeContextCurrent(NULL);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<skill_shot_type>(skill_shot_type type) {
    alSourcei(_skill_shot_achieved_model[type].sources, AL_BUFFER, _skill_shot_achieved_model[type].buffer);
    alSourceStop(_skill_shot_achieved_model[type].sources);
    alSourceRewind(_skill_shot_achieved_model[type].sources);
    alSourcePlay(_skill_shot_achieved_model[type].sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<tutorial_type>(tutorial_type type) {
    playSound<skill_shot_type>(static_cast<skill_shot_type>(type));
}

template<> void ALRenderer::playSound<achievement_unlocked>() {
    alSourcei(_achievement_unlocked_model.sources, AL_BUFFER, _achievement_unlocked_model.buffer);
    alSourceStop(_achievement_unlocked_model.sources);
    alSourceRewind(_achievement_unlocked_model.sources);
    alSourcePlay(_achievement_unlocked_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<goal_completed>() {
    alSourcei(_goal_completed_model.sources, AL_BUFFER, _goal_completed_model.buffer);
    alSourceStop(_goal_completed_model.sources);
    alSourceRewind(_goal_completed_model.sources);
    alSourcePlay(_goal_completed_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<chip_passed>() {
    alSourcei(_chip_passed_model[_current_chip_passed_index].sources,
              AL_BUFFER, _chip_passed_model[_current_chip_passed_index].buffer);
    alSourcePlay(_chip_passed_model[_current_chip_passed_index].sources);
    _current_chip_passed_index = (_current_chip_passed_index + 1) % _chip_passed_buffer_count;
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<chip_charged>() {
    alSourcei(_chip_charged_model[_current_chip_charged_index].sources,
              AL_BUFFER, _chip_charged_model[_current_chip_charged_index].buffer);
    alSourcePlay(_chip_charged_model[_current_chip_charged_index].sources);
    _current_chip_charged_index = (_current_chip_charged_index + 1) % _chip_charged_buffer_count;
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<chip_hit>() {
    alSourcei(_chip_hit_model[_current_chip_hit_index].sources,
              AL_BUFFER, _chip_hit_model[_current_chip_hit_index].buffer);
    alSourcePlay(_chip_hit_model[_current_chip_hit_index].sources);
    _current_chip_hit_index = (_current_chip_hit_index + 1) % _chip_hit_buffer_count;
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<cushion_hit>() {
    alSourcei(_cushion_hit_model[_current_cushion_hit_index].sources,
              AL_BUFFER, _cushion_hit_model[_current_cushion_hit_index].buffer);
    alSourcePlay(_cushion_hit_model[_current_cushion_hit_index].sources);
    _current_cushion_hit_index = (_current_cushion_hit_index + 1) % _cushion_hit_buffer_count;
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<cue_hit>() {
    alSourcei(_cue_hit_model.sources, AL_BUFFER, _cue_hit_model.buffer);
    alSourcePlay(_cue_hit_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<sliding>() {
    alSourcei(_sliding_model.sources, AL_BUFFER, _sliding_model.buffer);
    alSourcePlay(_sliding_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::playSound<missed>() {
    ALint state = 0;
    alGetSourcei(_missed_model.sources, AL_SOURCE_STATE, &state);

    if (state == AL_PLAYING) {
        alSourceStop(_missed_model.sources);
        alSourceRewind(_missed_model.sources);
    }
    alSourcei(_missed_model.sources, AL_BUFFER, _missed_model.buffer);
    alSourcePlay(_missed_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::stopSound<sliding>() {
    alSourceStop(_sliding_model.sources);
    alSourceRewind(_sliding_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::stopSound<missed>() {
    alSourceStop(_missed_model.sources);
    alSourceRewind(_missed_model.sources);
    CHECK_AL_ERROR(__FUNCTION__)
}

template<> void ALRenderer::setVolume<sliding>(float volume) {
    _sliding_model.gain = volume / 20.f;
    if (!_muted && fabs(_sliding_model.gain - volume) > 1e-5f) {
        alSourcef(_sliding_model.sources, AL_GAIN, _sliding_model.gain);
    }
    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::pause() {
    for (unsigned short i = 0; i < skill_shot_type_range; ++i) {
        pause(&_skill_shot_achieved_model[i]);
    }
    for (unsigned short i = 0; i < _chip_passed_buffer_count; ++i) {
        pause(&_chip_passed_model[i]);
    }
    for (unsigned short i = 0; i < _chip_charged_buffer_count; ++i) {
        pause(&_chip_charged_model[i]);
    }
    for (unsigned short i = 0; i < _chip_hit_buffer_count; ++i) {
        pause(&_chip_hit_model[i]);
    }
    for (unsigned short i = 0; i < _cushion_hit_buffer_count; ++i) {
        pause(&_cushion_hit_model[i]);
    }
    pause(&_cue_hit_model);
    pause(&_sliding_model);
    pause(&_missed_model);
    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::resume() {
    restoreContext();

    for (unsigned short i = 0; i < skill_shot_type_range; ++i) {
        resume(&_skill_shot_achieved_model[i]);
    }
    for (unsigned short i = 0; i < _chip_passed_buffer_count; ++i) {
        resume(&_chip_passed_model[i]);
    }
    for (unsigned short i = 0; i < _chip_charged_buffer_count; ++i) {
        resume(&_chip_charged_model[i]);
    }
    for (unsigned short i = 0; i < _chip_hit_buffer_count; ++i) {
        resume(&_chip_hit_model[i]);
    }
    for (unsigned short i = 0; i < _cushion_hit_buffer_count; ++i) {
        resume(&_cushion_hit_model[i]);
    }
    resume(&_cue_hit_model);
    resume(&_sliding_model);
    resume(&_missed_model);
    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::mute() {
    if (!_muted) {
        _muted = true;

        for (unsigned short i = 0; i < skill_shot_type_range; ++i) {
            alSourcef(_skill_shot_achieved_model[i].sources, AL_GAIN, 0.f);
        }
        for (unsigned short i = 0; i < _chip_passed_buffer_count; ++i) {
            alSourcef(_chip_passed_model[i].sources, AL_GAIN, 0.f);
        }
        for (unsigned short i = 0; i < _chip_charged_buffer_count; ++i) {
            alSourcef(_chip_charged_model[i].sources, AL_GAIN, 0.f);
        }
        for (unsigned short i = 0; i < _chip_hit_buffer_count; ++i) {
            alSourcef(_chip_hit_model[i].sources, AL_GAIN, 0.f);
        }
        for (unsigned short i = 0; i < _cushion_hit_buffer_count; ++i) {
            alSourcef(_cushion_hit_model[i].sources, AL_GAIN, 0.f);
        }
        alSourcef(_achievement_unlocked_model.sources, AL_GAIN, 0.f);
        alSourcef(_goal_completed_model.sources, AL_GAIN, 0.f);
        alSourcef(_cue_hit_model.sources, AL_GAIN, 0.f);
        alSourcef(_sliding_model.sources, AL_GAIN, 0.f);
        alSourcef(_missed_model.sources, AL_GAIN, 0.f);
    }
    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::unmute() {
    if (_muted) {
        _muted = false;

        for (unsigned short i = 0; i < skill_shot_type_range; ++i) {
            alSourcef(_skill_shot_achieved_model[i].sources, AL_GAIN, _skill_shot_achieved_model[i].gain);
        }
        for (unsigned short i = 0; i < _chip_passed_buffer_count; ++i) {
            alSourcef(_chip_passed_model[i].sources, AL_GAIN, _chip_passed_model[i].gain);
        }
        for (unsigned short i = 0; i < _chip_charged_buffer_count; ++i) {
            alSourcef(_chip_charged_model[i].sources, AL_GAIN, _chip_charged_model[i].gain);
        }
        for (unsigned short i = 0; i < _chip_hit_buffer_count; ++i) {
            alSourcef(_chip_hit_model[i].sources, AL_GAIN, _chip_hit_model[i].gain);
        }
        for (unsigned short i = 0; i < _cushion_hit_buffer_count; ++i) {
            alSourcef(_cushion_hit_model[i].sources, AL_GAIN, _cushion_hit_model[i].gain);
        }
        alSourcef(_achievement_unlocked_model.sources, AL_GAIN, _achievement_unlocked_model.gain);
        alSourcef(_goal_completed_model.sources, AL_GAIN, _goal_completed_model.gain);
        alSourcef(_cue_hit_model.sources, AL_GAIN, _cue_hit_model.gain);
        alSourcef(_sliding_model.sources, AL_GAIN, _sliding_model.gain);
        alSourcef(_missed_model.sources, AL_GAIN, _missed_model.gain);
        playSound<sound_type::cue_hit>();
    }

    CHECK_AL_ERROR(__FUNCTION__)
}

void ALRenderer::clear() {
    alDeleteModel(_skill_shot_achieved_model);
    alDeleteModel(_chip_passed_model, _chip_passed_buffer_count);
    alDeleteModel(_chip_charged_model, _chip_charged_buffer_count);
    alDeleteModel(_chip_hit_model, _chip_hit_buffer_count);
    alDeleteModel(_cushion_hit_model, _cushion_hit_buffer_count);
    alDeleteModel(&_achievement_unlocked_model);
    alDeleteModel(&_goal_completed_model);
    alDeleteModel(&_cue_hit_model);
    alDeleteModel(&_sliding_model);
    alDeleteModel(&_missed_model);
}

void ALRenderer::alDeleteModel(ALModel *model, unsigned short count) {
    if (count == 1) {
        alDeleteBuffers(1, &model->buffer);
        alDeleteSources(1, &model->sources);
    } else {
        for (unsigned short i = 0; i < count; ++i) {
            alDeleteBuffers(1, &model[i].buffer);
            alDeleteSources(1, &model[i].sources);
        }
    }
}

void ALRenderer::alSetupModel(IResourceLoader *resource_loader, ALModel *model, const char *filename, unsigned short count) {
    ALsizei bytes;
    void *data = nullptr;

    resource_loader->loadAudio(&data, &bytes, filename);

    if (count == 1) {
        alGenSources(1, &model->sources);
        alGenBuffers(1, &model->buffer);
        alBufferData(model->buffer, AL_FORMAT_STEREO16, data, bytes, 44100);
    } else {
        for (unsigned short i = 0; i < count; ++i) {
            alGenSources(1, &model[i].sources);
            alGenBuffers(1, &model[i].buffer);
            alBufferData(model[i].buffer, AL_FORMAT_STEREO16, data, bytes, 44100);
        }
    }
    if (data) {
        free(data);
        data = nullptr;
    }
}

void ALRenderer::pause(ALModel *model) {
    ALint state = 0;
    alGetSourcei(model->sources, AL_SOURCE_STATE, &state);

    if (state == AL_PLAYING) {
        alSourcei(model->sources, AL_BUFFER, model->buffer);
        alSourcePause(model->sources);
    }
}

void ALRenderer::resume(ALModel *model) {
    ALint state = 0;
    alGetSourcei(model->sources, AL_SOURCE_STATE, &state);

    if (state == AL_PAUSED) {
        alSourcei(model->sources, AL_BUFFER, model->buffer);
        alSourcePlay(model->sources);
    }
}
