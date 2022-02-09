// Copyright © 2018 Igor Pener. All rights reserved.

#ifndef GLRENDERER_HPP
#define GLRENDERER_HPP

#include <vector>
#include "assets.hpp"
#include "Chip.hpp"
#include "color.hpp"
#include "GLModel.hpp"
#include "IRenderer.hpp"
#include "matrix.hpp"
#include "shaders.hpp"

class GLRenderer : public IRenderer {
public:
    GLRenderer(class IResourceLoader *resource_loader, float scale_factor);

    ~GLRenderer();

    void prepareFrame() override;

    void prepareLineDraw() override;

    void prepareCircleDraw() override;

    void prepareLabelDraw() override;

    void prepareQuadBlending(float delta) override;

    void drawLine(const Line &l, float progress) override;

    void drawLine(const Line &l) override;

    void drawCircle(const Circle &c, float thickness) override;

    void drawHalo(const vec2<double> &p, float size, float thickness, const float color[4]) override;

    void drawCue(const vec2<double> &p, float alpha) override;

    void drawBlendQuad(const vec2<float> points[4], const float alphas[4], bool white) override;

    void drawBlendSpot(const vec2<float> &p, bool white, float alpha) override;

    void drawShadows(const std::vector<Chip> &chips) override;

    void drawChips(const std::vector<Chip> &chips) override;

    void drawLabels(const std::vector<Label> &labels) override;

    void setBackgroundGradient(const float gradient[6]);

    void setBlendEquation(bool additive) override;

    void setZoomData(const vec3<float> &data) override;

    void setBlurData(view_type type, const vec4<float> &data) override;

    void loadGLResources() override;

    void pause();

private:
    void setUpModels();

    void setUpTextureAtlas();

    void setUpFramebuffers();

    void setUpShaders();

    bool loadProgram(IGLShaders *shader, const std::string &vsh, const std::string &fsh);

    bool compileShader(GLuint *shader, GLenum type, const std::string &str);

    GLint getShaderStatus(GLuint id, GLenum status, void(*attrib_func)(GLuint, GLenum, GLint *), void(*info_func)(GLuint, GLsizei, GLsizei *, GLchar *));

    template<typename ... T>
    GLuint setUpBuffer(GLModel *model, const GLVertex<T...> *data, GLsizeiptr size, GLenum usage);

    template<typename ... T>
    void setUpInstanceBuffer(GLInstancedModel *model, const GLVertex<T...> *data, GLsizeiptr size, GLuint index);

    void setUpBuffer(GLModel *model, const unsigned short *data, GLsizeiptr size, GLenum usage);

    void setUpTexture(GLuint *texture, GLsizei width, GLsizei height, void *data);

    void updateTintShader(const rect<float> &tex_offset, const vec3<float> &label_data, const mat4 &mvp_mat);

    void deleteModel(const GLModel &model);

    void deleteShaders(const IGLShaders &shaders);

    void clearFramebufferTexture(GLuint texture);

private:
    static const GLsizei _line_vertex_count = 2;
    static const GLsizei _quad_vertex_count = 4;
    static const GLsizei _disc_vertex_count = 64;
    static const GLsizei _disc_index_count = 2 * (_disc_vertex_count + 1) + _disc_vertex_count;
    static const GLsizei _gradient_vertex_count = 10;
    static const GLsizei _alert_vertex_count = 16;
    static const GLsizei _tint_shader_instance_count = 256; // Note: Never draw more than 256 labels
    static const GLsizei _chip_shader_instance_count = 8; // Note: Never draw more than 5 chips

    class IResourceLoader *_resource_loader = nullptr;

    GLShaders<uniforms::mvp_mat_t, uniforms::sampler_t, uniforms::color_t>      _overlay_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::sampler_t, uniforms::data_t>       _reduce_overlay_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::sampler_t>                         _texture_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::sampler_t, uniforms::tex_offset_t> _blur_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::data_t, uniforms::sampler_t>       _mask_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::color_t>                           _color_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::color_t, uniforms::data_t>         _line_shaders;
    GLShaders<uniforms::mvp_mat_t, uniforms::color_t, uniforms::data_t>         _circle_shaders;
    GLShaders<uniforms::mvp_mat_t>                                              _gradient_shaders;
    GLShaders<uniforms::sampler_t, uniforms::resolution_t>                      _tint_shaders;
    GLShaders<uniforms::proj_mat_t, uniforms::sampler_t, uniforms::light_pos_t> _chip_shaders;
    
    GLVertex<vec2<float>, vec4<float>>                    _blend_quad_vertices[_quad_vertex_count];
    GLVertex<vec4<float>, vec3<float>, mat4>              _tint_shader_instances[_tint_shader_instance_count];
    GLVertex<vec4<float>, vec4<float>, mat4, mat3, float> _chip_shader_instances[_chip_shader_instance_count];

    GLInstancedModel _chip_model;
    GLInstancedModel _glyph_model;
    GLModel          _progress_line_model;
    GLModel          _cue_model;
    GLModel          _blend_quad_model;
    GLModel          _gradient_model;
    GLModel          _screen_model;
    GLModel          _line_model;
    GLModel          _circle_model;
    GLModel          _ui_small_alert_model;
    GLModel          _ui_large_alert_model;
    GLModel          _triangle_model;

    mat4             _ui_projection_matrix;
    mat4             _ui_modelview_matrix;
    mat4             _ui_modelview_projection_matrix;
    mat4             _projection_matrix;
    mat4             _modelview_matrix;
    mat4             _modelview_projection_matrix;
    vec2<GLsizei>    _resolution;
    vec2<GLsizei>    _quarter_resolution;
    vec3<float>      _light_position;

    GLshort          _tint_shader_current_instance_count = 0;
    GLint            _default_framebuffer = 0;
    GLuint           _framebuffer_textures[4] = {0, 0, 0, 0};
    GLuint           _framebuffers[2] = {0, 0};
    GLuint           _renderbuffers[2] = {0, 0};
    GLuint           _texture_atlas = 0;
    GLfloat          _texture_offset[2];

    unsigned short   _composition_texture_index = 0;
    unsigned short   _retained_texture_index = 0;
    float            _scale_factor;
    float            _zoom_factor = 1.f;
};

#endif
