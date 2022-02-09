// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GLMODEL_HPP
#define GLMODEL_HPP

#define GLES_SILENCE_DEPRECATION

#ifdef __APPLE__
#include <OpenGLES/ES3/glext.h>
#else
#include <GLES3/gl3.h>
#endif

#define UNIFORM(uniform)\
struct uniform##_t {\
    GLuint uniform = 0;\
protected:\
    uniform##_t(GLuint program) {\
        uniform = glGetUniformLocation(program, #uniform);\
    }\
}

namespace uniforms {
    UNIFORM(proj_mat);
    UNIFORM(mvp_mat);
    UNIFORM(tex_offset);
    UNIFORM(resolution);
    UNIFORM(color);
    UNIFORM(data);
    UNIFORM(light_pos);
    UNIFORM(sampler);
}

struct GLModel {
    GLuint vertex_array = 0;
    GLuint vertex_buffer = 0;
    GLuint index_buffer = 0;
};

struct GLInstancedModel : public GLModel {
    GLuint instance_buffer = 0;
};

struct IGLShaders {
    virtual void getUniforms() = 0;

    GLuint program = 0;
};

template<typename ... T>
struct GLShaders : IGLShaders {
    struct uniforms_t : T... {
        uniforms_t(GLuint program) : T(program)... {
        }
    };

    ~GLShaders() {
        if (uniforms) {
            delete uniforms;
        }
    }

    void getUniforms() override {
        uniforms = new uniforms_t(program);
    }

    uniforms_t *uniforms = nullptr;
};

template<typename ... T>
using GLVertex = std::tuple<T...>;

#endif
