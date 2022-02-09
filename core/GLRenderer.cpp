// Copyright © 2018 Igor Pener. All rights reserved.

#include "GLRenderer.hpp"

#include <exception>
#include <cstdlib>
#include <string>
#include "Chip.hpp"
#include "IResourceLoader.hpp"
#include "Logger.hpp"

using namespace std;
using namespace assets;
using namespace shaders;

// TODO: Use following call to retrieve the preferred texture format (hardware-specific)
// glGetInternalFormativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &preferred_format).

GLRenderer::GLRenderer(IResourceLoader *resource_loader, float scale_factor) :
    _resource_loader(resource_loader), _scale_factor(scale_factor) {
    _resolution = {static_cast<GLsizei>(screen_size.x * scale_factor), static_cast<GLsizei>(screen_size.y * scale_factor)};
    _quarter_resolution = _resolution / 4;
    setZoomData({screen_size.x * 0.5f, screen_size.y * 0.5f, screen_size.y * 0.5f});
    _texture_offset[0] = _resolution.y / static_cast<float>(_resolution.x);
    _texture_offset[1] = 8.f * _scale_factor / _resolution.x;
    _ui_projection_matrix = _projection_matrix;
    _ui_modelview_matrix = _modelview_matrix;
    _ui_modelview_projection_matrix = _ui_projection_matrix * _ui_modelview_matrix;
}

GLRenderer::~GLRenderer() {
    try {
        deleteModel(_blend_quad_model);
        deleteModel(_progress_line_model);
        deleteModel(_cue_model);
        deleteModel(_gradient_model);
        deleteModel(_screen_model);
        deleteModel(_chip_model);
        deleteModel(_line_model);
        deleteModel(_circle_model);
        deleteModel(_ui_small_alert_model);
        deleteModel(_ui_large_alert_model);
        deleteModel(_chip_model);
        deleteModel(_triangle_model);
        deleteShaders(_color_shaders);
        deleteShaders(_line_shaders);
        deleteShaders(_circle_shaders);
        deleteShaders(_gradient_shaders);
        deleteShaders(_tint_shaders);
        deleteShaders(_texture_shaders);
        deleteShaders(_blur_shaders);
        deleteShaders(_overlay_shaders);
        deleteShaders(_reduce_overlay_shaders);
        deleteShaders(_chip_shaders);
        glDeleteFramebuffers(2, _framebuffers);
        glDeleteRenderbuffers(2, _renderbuffers);
        glDeleteTextures(4, _framebuffer_textures);
        glDeleteTextures(1, &_texture_atlas);

        GLenum error = GL_NO_ERROR;
        while ((error = glGetError()) != GL_NO_ERROR) {
            loge << "OpenGL error occured: " << error << endl;
        }
    } catch (const exception &e) {
        loge << "Exception occured in ~GLRenderer: " << e.what() << endl;
    }
}

void GLRenderer::prepareFrame() {
    glDisable(GL_BLEND);
    glBindVertexArray(_gradient_model.vertex_array);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[0]);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index]);
    glUseProgram(_overlay_shaders.program);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    glDrawArrays(GL_TRIANGLE_FAN, 0, _gradient_vertex_count);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
    glActiveTexture(GL_TEXTURE0);
}

void GLRenderer::prepareLineDraw() {
    glUseProgram(_color_shaders.program);
    glBindVertexArray(_line_model.vertex_array);
}

void GLRenderer::prepareCircleDraw() {
    glUseProgram(_circle_shaders.program);
    glBindVertexArray(_circle_model.vertex_array);
}

void GLRenderer::prepareLabelDraw() {
    glBindVertexArray(_glyph_model.vertex_array);
}

void GLRenderer::prepareQuadBlending(float delta) {
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffers[1]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index ^ 1], 0);

    const GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
    glBlitFramebuffer(0, 0, _resolution.x, _resolution.y, 0, 0, _resolution.x, _resolution.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index]);
    glBindVertexArray(_screen_model.vertex_array);
    glUseProgram(_reduce_overlay_shaders.program);
    glUniform1f(_reduce_overlay_shaders.uniforms->data, delta);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
    glActiveTexture(GL_TEXTURE0);
}

void GLRenderer::drawLine(const Line &l, float progress) {
    glBindVertexArray(_progress_line_model.vertex_array);
    glUseProgram(_line_shaders.program);

    if (progress < 0.99999f) {
        const mat4 mv_mat = _ui_modelview_matrix * mat4::scaleMat(l.size, ui_progress_line_thickness, 1.f).translate(l.p.x, l.p.y, 0.f);
        const mat4 mvp_mat = _ui_projection_matrix * mv_mat;
        glUniformMatrix4fv(_line_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
        glUniform4f(_line_shaders.uniforms->color, 0.f, 0.f, 0.f, l.alpha * ui_disabled_alpha);
        glUniform1f(_line_shaders.uniforms->data, l.size / ui_progress_line_thickness);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
    }
    if (progress > 1e-5f) {
        const mat4 mv_mat = _ui_modelview_matrix * mat4::scaleMat(l.size * progress, ui_progress_line_thickness, 1.f).translate(l.p.x - (l.size - l.size * progress) / 2.f, l.p.y, 0.f);
        const mat4 mvp_mat = _ui_projection_matrix * mv_mat;
        glUniformMatrix4fv(_line_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
        glUniform4f(_line_shaders.uniforms->color, 0.f, 0.f, 0.f, l.alpha);
        glUniform1f(_line_shaders.uniforms->data, l.size * progress / ui_progress_line_thickness);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
    }
}

void GLRenderer::drawLine(const Line &l) {
    const mat4 mv_mat = _ui_modelview_matrix * mat4::scaleMat(l.size, 1.f, 1.f).translate(l.p.x, l.p.y, 0.f);
    const mat4 mvp_mat = _ui_projection_matrix * mv_mat;

    glUniformMatrix4fv(_color_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
    glUniform4f(_color_shaders.uniforms->color, 0.f, 0.f, 0.f, l.alpha);
    glDrawArrays(GL_LINES, 0, _line_vertex_count);
}

void GLRenderer::drawCircle(const Circle &c, float thickness) {
    const mat4 mv_mat = _ui_modelview_matrix * mat4::scaleMat(c.size, c.size, 1.f).translate(c.p.x, c.p.y, 0.f);
    const mat4 mvp_mat = _ui_projection_matrix * mv_mat;

    glUniformMatrix4fv(_circle_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
    glUniform3f(_circle_shaders.uniforms->data, thickness / c.size, 2.f * M_PI * c.progress, ui_disabled_alpha);
    glUniform4f(_circle_shaders.uniforms->color, c.color.r, c.color.g, c.color.b, c.color.a);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _disc_vertex_count * 2 + 2);
}

void GLRenderer::drawHalo(const vec2<double> &p, float size, float thickness, const float color[4]) {
    const mat4 mv_mat = _modelview_matrix * mat4::scaleMat(size, size, 1.f).translate(p.x, p.y, 0.f);
    const mat4 mvp_mat = _projection_matrix * mv_mat;

    glUseProgram(_circle_shaders.program);
    glBindVertexArray(_circle_model.vertex_array);
    glUniformMatrix4fv(_circle_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
    glUniform3f(_circle_shaders.uniforms->data, thickness / size, 2.f * M_PI, ui_disabled_alpha);
    glUniform4fv(_circle_shaders.uniforms->color, 1, color);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _disc_vertex_count * 2 + 2);
}

void GLRenderer::drawCue(const vec2<double> &p, float alpha) {
    const mat4 mvp_mat = _projection_matrix * _modelview_matrix.translate(p.x, p.y, 0.f);

    glUseProgram(_color_shaders.program);
    glUniformMatrix4fv(_color_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
    glUniform4f(_color_shaders.uniforms->color, 0.f, 0.f, 0.f, alpha);
    glBindVertexArray(_cue_model.vertex_array);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _disc_vertex_count);
}

void GLRenderer::drawBlendQuad(const vec2<float> points[4], const float alphas[4], bool white) {
    setBlendEquation(white);
    for (short i = 0; i < 4; ++i) {
        get<0>(_blend_quad_vertices[i]).x = points[i].x;
        get<0>(_blend_quad_vertices[i]).y = points[i].y;
        get<1>(_blend_quad_vertices[i]).w = alphas[i];
    }

    glBindVertexArray(_blend_quad_model.vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, _blend_quad_model.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_blend_quad_vertices), _blend_quad_vertices, GL_DYNAMIC_DRAW);
    glUseProgram(_gradient_shaders.program);
    glUniformMatrix4fv(_gradient_shaders.uniforms->mvp_mat, 1, 0, _modelview_projection_matrix.m);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void GLRenderer::drawBlendSpot(const vec2<float> &p, bool white, float alpha) {
    setBlendEquation(white);
    const auto &uv = texture_atlas.find(glyphs::blend_spot)->second.uv[0];
    const mat4 mvp_mat = _projection_matrix * _modelview_matrix * mat4::scaleMat(200.f, 200.f, 1.f).translate(p.x, p.y, 0.f);
    _tint_shader_current_instance_count = 0;

    updateTintShader(uv, {-1.f, alpha, 0.f}, mvp_mat);

    glBindVertexArray(_glyph_model.vertex_array);
    glUseProgram(_tint_shaders.program);
    glBindBuffer(GL_ARRAY_BUFFER, _glyph_model.instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, _tint_shader_current_instance_count * sizeof(_tint_shader_instances[0]), _tint_shader_instances, GL_DYNAMIC_DRAW);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, _quad_vertex_count, _tint_shader_current_instance_count);
    glDisable(GL_BLEND);
}

void GLRenderer::drawShadows(const std::vector<Chip> &chips) {
    const auto &uv = texture_atlas.find(glyphs::shadow)->second.uv[0];
    const float sy = 2.f * Chip::default_size;
    const float sx = uv.size.x / uv.size.y * sy;
    _tint_shader_current_instance_count = 0;
    mat4 mvp_mat;

    for (const auto &chip : chips) {
        mvp_mat = _projection_matrix * _modelview_matrix * mat4::scaleMat(sx, sy, 1.f).rotateZ(M_PI_4).translate(chip.p.x - Chip::default_size, chip.p.y - Chip::default_size, 0.f);
        updateTintShader(uv, {-1.f, 1.f, 0.f}, mvp_mat);
    }

    glUseProgram(_tint_shaders.program);
    glBindBuffer(GL_ARRAY_BUFFER, _glyph_model.instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, _tint_shader_current_instance_count * sizeof(_tint_shader_instances[0]), _tint_shader_instances, GL_DYNAMIC_DRAW);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, _quad_vertex_count, _tint_shader_current_instance_count);
}

void GLRenderer::drawChips(const std::vector<Chip> &chips) {
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(_chip_model.vertex_array);
    glUseProgram(_chip_shaders.program);
    glUniformMatrix4fv(_chip_shaders.uniforms->proj_mat, 1, 0, _projection_matrix.m);

    const auto size = static_cast<GLsizei>(chips.size());
    rect<float> tex_offset;
    mat4 mv_mat;
    GLint level;

    for (short i = 0; i < size; ++i) {
        if (chips[i].type == chip_type_default) {
            level = 0;
        } else {
            level = chips[i].level > 0 ? chips[i].level - 1 : chips[i].level;
        }
        tex_offset = texture_atlas.find(chips[i].gradient)->second.uv[level];
        get<0>(_chip_shader_instances[i]).x = tex_offset.origin.x;
        get<0>(_chip_shader_instances[i]).y = tex_offset.origin.y;
        get<0>(_chip_shader_instances[i]).z = tex_offset.size.x;
        get<0>(_chip_shader_instances[i]).w = tex_offset.size.y;

        tex_offset = texture_atlas.find(chips[i].icon)->second.uv[level];
        get<1>(_chip_shader_instances[i]).x = tex_offset.origin.x;
        get<1>(_chip_shader_instances[i]).y = tex_offset.origin.y;
        get<1>(_chip_shader_instances[i]).z = tex_offset.size.x;
        get<1>(_chip_shader_instances[i]).w = tex_offset.size.y;

        mv_mat = _modelview_matrix.translate(chips[i].p.x, chips[i].p.y, 0.f);
        get<2>(_chip_shader_instances[i]) = mv_mat;
        get<3>(_chip_shader_instances[i]) = mv_mat.getMat3().invertAndTranspose();
        get<4>(_chip_shader_instances[i]) = static_cast<float>(level);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _chip_model.instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(_chip_shader_instances[0]), _chip_shader_instances, GL_DYNAMIC_DRAW);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, _disc_index_count, GL_UNSIGNED_SHORT, 0, size);
    glDisable(GL_DEPTH_TEST);
}

void GLRenderer::drawLabels(const vector<Label> &labels) {
    _tint_shader_current_instance_count = 0;
    mat4 mv_mat;

    for (const auto &label : labels) {
        if (fabs(label.alpha) < 1e-5f || (label.text.empty() && label.type == fill_type::regular))
            continue;

        if (label.type != fill_type::regular) {
            mv_mat = _ui_modelview_matrix * mat4::scaleMat(label.bounds.size.x, label.bounds.size.y, 1.f).translate(label.bounds.origin.x + label.bounds.size.x / 2.f, label.bounds.origin.y + label.bounds.size.y / 2.f, 0.f);
            mat4 mvp_mat = _ui_projection_matrix * mv_mat;

            if (label.type == fill_type::inverted) {
                glUseProgram(_color_shaders.program);
                glUniformMatrix4fv(_color_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
                glUniform4f(_color_shaders.uniforms->color, 0.f, 0.f, 0.f, label.alpha);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
            } else {
                glUseProgram(_line_shaders.program);
                glUniformMatrix4fv(_line_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
                glUniform4f(_line_shaders.uniforms->color, 0.f, 0.f, 0.f, label.alpha);
                glUniform1f(_line_shaders.uniforms->data, label.bounds.size.x / label.bounds.size.y);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);

                if (fabs(label.pointer) > 0.f) {
                    const auto s = label.bounds.size.y / 3.f;
                    const auto x = label.bounds.origin.x + label.bounds.size.x * fabs(label.pointer);
                    const auto sx = fabs(label.pointer) < 0.5f ? s : -s;
                    const auto sy = label.pointer > 0.f ? s : -s;
                    const auto ty = label.pointer > 0.f ? label.bounds.origin.y - s / 2.f : label.bounds.origin.y + label.bounds.size.y + s / 2.f;
                    mv_mat = _ui_modelview_matrix * mat4::scaleMat(sx, sy, 1.f).translate(x, ty, 0.f);
                    mvp_mat = _ui_projection_matrix * mv_mat;

                    glBindVertexArray(_triangle_model.vertex_array);
                    glUseProgram(_color_shaders.program);
                    glUniformMatrix4fv(_color_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
                    glUniform4f(_color_shaders.uniforms->color, 0.f, 0.f, 0.f, label.alpha);
                    glDisable(GL_CULL_FACE);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
                    glBindVertexArray(_glyph_model.vertex_array);
                    glEnable(GL_CULL_FACE);
                }
            }
        }

        for (const auto &c : label.text) {
            mv_mat = _ui_modelview_matrix * mat4::scaleMat(c.size.x, c.size.y, 1.f).translate(c.origin.x + c.size.x / 2.f, c.origin.y + c.size.y / 2.f, 0.f);
            updateTintShader(*c.uv, {label.type == fill_type::regular ? -1.f : 1.f, label.alpha, static_cast<float>(c.weight)}, _ui_projection_matrix * mv_mat);
        }
    }

    glUseProgram(_tint_shaders.program);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_composition_texture_index]);
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, _glyph_model.instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, _tint_shader_current_instance_count * sizeof(_tint_shader_instances[0]), _tint_shader_instances, GL_DYNAMIC_DRAW);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, _quad_vertex_count, _tint_shader_current_instance_count);
}

void GLRenderer::setBackgroundGradient(const float gradient[6]) {
    glUseProgram(_overlay_shaders.program);
    glUniform3fv(_overlay_shaders.uniforms->color, 2, gradient);
}

void GLRenderer::setBlendEquation(bool additive) {
    glEnable(GL_BLEND);
    if (additive) {
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void GLRenderer::setZoomData(const vec3<float> &data) {
    const float alpha = 130.f * M_PI / 180.f;
    const float depth = data.z / tan(alpha * 0.5f);
    _zoom_factor = screen_size.y * 0.5f / data.z;
    _projection_matrix = mat4::perspective(alpha, screen_size.x / screen_size.y, depth - 2.1f, depth + 0.001f);
    _modelview_matrix = mat4::translation(-data.x, -data.y, -depth);
    _modelview_projection_matrix = _projection_matrix * _modelview_matrix;
    _light_position = {static_cast<float>(_resolution.x), static_cast<float>(_resolution.y), depth};
}

void GLRenderer::setBlurData(view_type type, const vec4<float> &data) {
    glDisable(GL_BLEND);
    glBindVertexArray(_screen_model.vertex_array);
    glActiveTexture(GL_TEXTURE1);

    if (fabs(data.z) > 1e-5f) {
        const float s = 4.f * data.z * data.z;

        glUseProgram(_blur_shaders.program);

        if (data.z < 0.5f) {
            clearFramebufferTexture(_framebuffer_textures[_retained_texture_index]);

            glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index ^ 1]);
            glUniform3f(_blur_shaders.uniforms->tex_offset, 0.f, s / _resolution.y, data.z);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _default_framebuffer);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index]);
            glUniform3f(_blur_shaders.uniforms->tex_offset, s / _resolution.x, 0.f, data.z);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
            _composition_texture_index = _retained_texture_index;
        } else {
            clearFramebufferTexture(_framebuffer_textures[3]);

            glUniform3f(_blur_shaders.uniforms->tex_offset, 0.f, s / _resolution.y, data.z);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            glViewport(0, 0, _quarter_resolution.x, _quarter_resolution.y);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);

            clearFramebufferTexture(_framebuffer_textures[2]);

            glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[3]);
            glUniform3f(_blur_shaders.uniforms->tex_offset, s / _resolution.x, 0.f, data.z);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);

            glViewport(0, 0, _resolution.x, _resolution.y);
            glUseProgram(_texture_shaders.program);

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _default_framebuffer);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
            if (type != view_type::full_screen) {
                const mat4 mvp_mat = _projection_matrix * _modelview_matrix.translate(data.x, data.y, 0.f);

                glBindVertexArray(_screen_model.vertex_array);
                glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index]);
                glUseProgram(_texture_shaders.program);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);

                if (type == view_type::small_alert) {
                    glBindVertexArray(_ui_small_alert_model.vertex_array);
                } else {
                    glBindVertexArray(_ui_large_alert_model.vertex_array);
                }
                glUseProgram(_mask_shaders.program);
                glUniformMatrix4fv(_mask_shaders.uniforms->mvp_mat, 1, 0, mvp_mat.m);
                glUniform4f(_mask_shaders.uniforms->data, data.x, data.y, screen_size.x, screen_size.y);
                glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[2]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _alert_vertex_count);
            } else {
                glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[2]);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
            }
            _composition_texture_index = 2;
        }
    } else {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _default_framebuffer);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index ^ 1]);
        glUseProgram(_texture_shaders.program);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, _quad_vertex_count);
        _composition_texture_index = _retained_texture_index ^ 1;
    }

    _retained_texture_index ^= 1;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
}

void GLRenderer::loadGLResources() {
    setUpShaders();
    setUpModels();
}

void GLRenderer::pause() {
    glFinish();
}

void GLRenderer::setUpModels() {
    const GLVertex<vec2<float>, vec2<float>> screen_vertices[_quad_vertex_count] {
        GLVertex<vec2<float>, vec2<float>>({screen_size.x, screen_size.y}, {1.0f, 1.0f}),
        GLVertex<vec2<float>, vec2<float>>({          0.f, screen_size.y}, {0.0f, 1.0f}),
        GLVertex<vec2<float>, vec2<float>>({screen_size.x,           0.f}, {1.0f, 0.0f}),
        GLVertex<vec2<float>, vec2<float>>({          0.f,           0.f}, {0.0f, 0.0f})
    };
    const auto w = screen_size / sqrt(screen_size.x * screen_size.x + screen_size.y * screen_size.y);
    const GLVertex<vec2<float>, vec3<float>> gradient_vertices[_gradient_vertex_count] {
        GLVertex<vec2<float>, vec3<float>>({screen_size.x / 2.f, screen_size.y / 2.f}, {0.5f, 0.5f, 0.f}),
        GLVertex<vec2<float>, vec3<float>>({      screen_size.x, screen_size.y / 2.f}, {1.0f, 0.5f, w.x}),
        GLVertex<vec2<float>, vec3<float>>({      screen_size.x,       screen_size.y}, {1.0f, 1.0f, 1.f}),
        GLVertex<vec2<float>, vec3<float>>({screen_size.x / 2.f,       screen_size.y}, {0.5f, 1.0f, w.y}),
        GLVertex<vec2<float>, vec3<float>>({                0.f,       screen_size.y}, {0.0f, 1.0f, 1.f}),
        GLVertex<vec2<float>, vec3<float>>({                0.f, screen_size.y / 2.f}, {0.0f, 0.5f, w.x}),
        GLVertex<vec2<float>, vec3<float>>({                0.f,                 0.f}, {0.0f, 0.0f, 1.f}),
        GLVertex<vec2<float>, vec3<float>>({screen_size.x / 2.f,                 0.f}, {0.5f, 0.0f, w.y}),
        GLVertex<vec2<float>, vec3<float>>({      screen_size.x,                 0.f}, {1.0f, 0.0f, 1.f}),
        GLVertex<vec2<float>, vec3<float>>({      screen_size.x, screen_size.y / 2.f}, {1.0f, 0.5f, w.x})
    };
    const GLVertex<vec2<float>, vec2<float>> glyph_vertices[_quad_vertex_count] {
        GLVertex<vec2<float>, vec2<float>>({-0.5f, -0.5f}, {0.0f, 1.0f}),
        GLVertex<vec2<float>, vec2<float>>({ 0.5f, -0.5f}, {1.0f, 1.0f}),
        GLVertex<vec2<float>, vec2<float>>({-0.5f,  0.5f}, {0.0f, 0.0f}),
        GLVertex<vec2<float>, vec2<float>>({ 0.5f,  0.5f}, {1.0f, 0.0f})
    };
    const GLVertex<vec2<float>> progress_line_vertices[_quad_vertex_count] {
        GLVertex<vec2<float>>({-0.5f, -0.5f}),
        GLVertex<vec2<float>>({ 0.5f, -0.5f}),
        GLVertex<vec2<float>>({-0.5f,  0.5f}),
        GLVertex<vec2<float>>({ 0.5f,  0.5f})
    };
    const GLVertex<vec2<float>> triangle_vertices[3] {
        GLVertex<vec2<float>>({-0.5f, -0.5f}),
        GLVertex<vec2<float>>({ 0.5f,  0.5f}),
        GLVertex<vec2<float>>({-0.5f,  0.5f})
    };
    const GLVertex<vec2<float>> line_vertices[_line_vertex_count] {
        GLVertex<vec2<float>>({-0.5f, 0.f}),
        GLVertex<vec2<float>>({ 0.5f, 0.f})
    };
    vec2<float> alert_size((screen_size.x - 2.f * ui_margin) / 2.f, ui_small_alert_height / 2.f);
    const float corner_radius = 6.f;
    const auto d30 = vec2<float>(1.f - cos(M_PI / 6.f), 1.f - sin(M_PI / 6.f)) * corner_radius;
    const auto d60 = vec2<float>(1.f - cos(M_PI / 3.f), 1.f - sin(M_PI / 3.f)) * corner_radius;
    const GLVertex<vec2<float>> ui_small_alert_vertices[_alert_vertex_count] {
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x, screen_size.y / 2.f - alert_size.y + corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x, screen_size.y / 2.f + alert_size.y - corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d30.x, screen_size.y / 2.f - alert_size.y + d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d30.x, screen_size.y / 2.f + alert_size.y - d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d60.x, screen_size.y / 2.f - alert_size.y + d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d60.x, screen_size.y / 2.f + alert_size.y - d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - corner_radius, screen_size.y / 2.f - alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - corner_radius, screen_size.y / 2.f + alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + corner_radius, screen_size.y / 2.f - alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + corner_radius, screen_size.y / 2.f + alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d60.x, screen_size.y / 2.f - alert_size.y + d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d60.x, screen_size.y / 2.f + alert_size.y - d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d30.x, screen_size.y / 2.f - alert_size.y + d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d30.x, screen_size.y / 2.f + alert_size.y - d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x, screen_size.y / 2.f - alert_size.y + corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x, screen_size.y / 2.f + alert_size.y - corner_radius})
    };
    alert_size.y = ui_large_alert_height / 2.f;
    const GLVertex<vec2<float>> ui_large_alert_vertices[_alert_vertex_count] {
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x, screen_size.y / 2.f - alert_size.y + corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x, screen_size.y / 2.f + alert_size.y - corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d30.x, screen_size.y / 2.f - alert_size.y + d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d30.x, screen_size.y / 2.f + alert_size.y - d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d60.x, screen_size.y / 2.f - alert_size.y + d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - d60.x, screen_size.y / 2.f + alert_size.y - d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - corner_radius, screen_size.y / 2.f - alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f + alert_size.x - corner_radius, screen_size.y / 2.f + alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + corner_radius, screen_size.y / 2.f - alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + corner_radius, screen_size.y / 2.f + alert_size.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d60.x, screen_size.y / 2.f - alert_size.y + d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d60.x, screen_size.y / 2.f + alert_size.y - d60.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d30.x, screen_size.y / 2.f - alert_size.y + d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x + d30.x, screen_size.y / 2.f + alert_size.y - d30.y}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x, screen_size.y / 2.f - alert_size.y + corner_radius}),
        GLVertex<vec2<float>>({screen_size.x / 2.f - alert_size.x, screen_size.y / 2.f + alert_size.y - corner_radius})
    };

    GLVertex<vec3<float>, vec3<float>, vec2<float>> chip_vertices[_disc_vertex_count * 3];
    GLVertex<vec2<float>> cue_vertices[_disc_vertex_count];
    GLVertex<vec2<float>> circle_vertices[_disc_vertex_count * 2 + 2];
    unsigned short chip_indices[_disc_index_count];
    for (short i = 0; i < _disc_vertex_count; ++i) {
        chip_indices[i] = i & 1 ? (i + 1) / 2 : (_disc_vertex_count - i / 2) % _disc_vertex_count;
    }
    for (short i = _disc_vertex_count, j = 0; i < _disc_index_count; i += 2, ++j) {
        chip_indices[i] = j + _disc_vertex_count;
        chip_indices[i + 1] = j + 2 * _disc_vertex_count;
    }
    chip_indices[3 * _disc_vertex_count] = _disc_vertex_count;
    chip_indices[3 * _disc_vertex_count + 1] = 2 * _disc_vertex_count;

    const float angle = 6.28318530718f / static_cast<double>(_disc_vertex_count);
    for (short i = 0; i < _disc_vertex_count; ++i) {
        const vec2<float> p(cos(angle * i), sin(angle * i));
        const vec2<float> s = p * static_cast<float>(Chip::default_size);

        get<0>(circle_vertices[2 * i]).x = p.x * 0.9f;
        get<0>(circle_vertices[2 * i]).y = p.y * 0.9f;
        get<0>(circle_vertices[2 * i + 1]).x = p.x * 1.1f;
        get<0>(circle_vertices[2 * i + 1]).y = p.y * 1.1f;
        get<0>(chip_vertices[i]).x = s.x;
        get<0>(chip_vertices[i]).y = s.y;
        get<0>(chip_vertices[i]).z = 2.f;
        get<0>(chip_vertices[i + _disc_vertex_count]).x = s.x;
        get<0>(chip_vertices[i + _disc_vertex_count]).y = s.y;
        get<0>(chip_vertices[i + _disc_vertex_count]).z = 2.f;
        get<0>(chip_vertices[i + 2 * _disc_vertex_count]).x = s.x;
        get<0>(chip_vertices[i + 2 * _disc_vertex_count]).y = s.y;
        get<0>(chip_vertices[i + 2 * _disc_vertex_count]).z = 0.f;
        get<1>(chip_vertices[i]).x = p.x * 0.001f;
        get<1>(chip_vertices[i]).y = p.y * 0.001f;
        get<1>(chip_vertices[i]).z = 1.f;
        get<1>(chip_vertices[i + _disc_vertex_count]).x = p.x;
        get<1>(chip_vertices[i + _disc_vertex_count]).y = p.y;
        get<1>(chip_vertices[i + _disc_vertex_count]).z = 0.f;
        get<1>(chip_vertices[i + 2 * _disc_vertex_count]).x = p.x;
        get<1>(chip_vertices[i + 2 * _disc_vertex_count]).y = p.y;
        get<1>(chip_vertices[i + 2 * _disc_vertex_count]).z = 0.f;
        get<2>(chip_vertices[i]).x = 0.5f + p.x * 0.5f;
        get<2>(chip_vertices[i]).y = 0.5f - p.y * 0.5f;
        get<2>(chip_vertices[i + _disc_vertex_count]).x = get<2>(chip_vertices[i]).x;
        get<2>(chip_vertices[i + _disc_vertex_count]).x = get<2>(chip_vertices[i]).y;
        get<2>(chip_vertices[i + 2 * _disc_vertex_count]).x = get<2>(chip_vertices[i]).x;
        get<2>(chip_vertices[i + 2 * _disc_vertex_count]).x = get<2>(chip_vertices[i]).y;

        if (i < _disc_vertex_count / 2) {
            get<0>(cue_vertices[2 * i + 1]).x = s.x;
            get<0>(cue_vertices[2 * i + 1]).y = s.y;
        } else {
            get<0>(cue_vertices[2 * (_disc_vertex_count - 1 - i)]).x = s.x;
            get<0>(cue_vertices[2 * (_disc_vertex_count - 1 - i)]).y = s.y;
        }
    }
    get<0>(circle_vertices[2 * _disc_vertex_count]).x = get<0>(circle_vertices[0]).x;
    get<0>(circle_vertices[2 * _disc_vertex_count]).y = get<0>(circle_vertices[0]).y;
    get<0>(circle_vertices[2 * _disc_vertex_count + 1]).x = get<0>(circle_vertices[1]).x;
    get<0>(circle_vertices[2 * _disc_vertex_count + 1]).y = get<0>(circle_vertices[1]).y;

    for (short i = 0; i < 4; ++i) {
        get<1>(_blend_quad_vertices[i]).x = 0.f;
        get<1>(_blend_quad_vertices[i]).y = 0.f;
        get<1>(_blend_quad_vertices[i]).z = 0.f;
    }

    setUpBuffer(&_blend_quad_model, _blend_quad_vertices, sizeof(_blend_quad_vertices), GL_DYNAMIC_DRAW);
    setUpBuffer(&_progress_line_model, progress_line_vertices, sizeof(progress_line_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_cue_model, cue_vertices, sizeof(cue_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_gradient_model, gradient_vertices, sizeof(gradient_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_screen_model, screen_vertices, sizeof(screen_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_line_model, line_vertices, sizeof(line_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_circle_model, circle_vertices, sizeof(circle_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_ui_small_alert_model, ui_small_alert_vertices, sizeof(ui_small_alert_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_ui_large_alert_model, ui_large_alert_vertices, sizeof(ui_large_alert_vertices), GL_STATIC_DRAW);
    setUpBuffer(&_triangle_model, triangle_vertices, sizeof(triangle_vertices), GL_STATIC_DRAW);
    setUpInstanceBuffer(&_glyph_model, _tint_shader_instances, sizeof(_tint_shader_instances),
        setUpBuffer(&_glyph_model, glyph_vertices, sizeof(glyph_vertices), GL_STATIC_DRAW));
    setUpInstanceBuffer(&_chip_model, _chip_shader_instances, sizeof(_chip_shader_instances),
        setUpBuffer(&_chip_model, chip_vertices, sizeof(chip_vertices), GL_STATIC_DRAW));
    setUpBuffer(&_chip_model, chip_indices, sizeof(chip_indices), GL_STATIC_DRAW);

    setUpTextureAtlas();
    setUpFramebuffers();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_atlas);
    glBlendColor(0.f, 0.f, 0.f, 0.5f);
    glLineWidth(_scale_factor);
}

void GLRenderer::setUpTextureAtlas() {
    GLubyte *data = nullptr;
    GLsizei width = 0, height = 0;
    _resource_loader->loadImage((void **)&data, &width, &height, "texture_atlas");

    if (width == static_cast<GLsizei>(texture_atlas_size.x) && height == static_cast<GLsizei>(texture_atlas_size.y)) {
        glGenTextures(1, &_texture_atlas);
        glBindTexture(GL_TEXTURE_2D, _texture_atlas);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        loge << "Unexpected texture_atlas.png with size (" << width << ", " << height << ") loaded" << endl;
    }
    if (data) {
        free(data);
    }
}

void GLRenderer::setUpFramebuffers() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_default_framebuffer);
    glGenFramebuffers(2, _framebuffers);
    glGenRenderbuffers(2, _renderbuffers);

    setUpTexture(&_framebuffer_textures[0], _resolution.x, _resolution.y, NULL);
    setUpTexture(&_framebuffer_textures[1], _resolution.x, _resolution.y, NULL);
    setUpTexture(&_framebuffer_textures[2], _quarter_resolution.x, _quarter_resolution.y, NULL);
    setUpTexture(&_framebuffer_textures[3], _quarter_resolution.x, _quarter_resolution.y, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[1]);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _framebuffer_textures[_retained_texture_index], 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        logi << "glCheckFramebufferStatus returned " << status << std::endl;
    }

    glViewport(0, 0, _resolution.x, _resolution.y);
    glClearColor(0.f, 0.f, 0.f, 0.5f);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT);

    GLsizei samples;
    if (_scale_factor + 1e-5f > 4.f) {
        samples = 1;
    } else if (_scale_factor + 1e-5f > 3.f) {
        samples = 2;
    } else {
        samples = 4;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffers[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffers[0]);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, _resolution.x, _resolution.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderbuffers[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderbuffers[1]);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT16, _resolution.x, _resolution.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderbuffers[1]);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        logi << "glCheckFramebufferStatus returned " << status << endl;
    }

    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::setUpShaders() {
    if (!loadProgram(&_color_shaders, color_vsh(), color_fsh())) {
        loge << "Failed to load \"color\" shaders" << endl;
    }
    if (!loadProgram(&_line_shaders, line_vsh(), line_fsh())) {
        loge << "Failed to load \"line\" shaders" << endl;
    }
    if (!loadProgram(&_circle_shaders, circle_vsh(), circle_fsh())) {
        loge << "Failed to load \"circle\" shaders" << endl;
    }
    if (!loadProgram(&_gradient_shaders, gradient_vsh(), gradient_fsh())) {
        loge << "Failed to load \"gradient\" shaders" << endl;
    }
    if (!loadProgram(&_tint_shaders, tint_vsh(), tint_fsh())) {
        loge << "Failed to load \"tint\" shaders" << endl;
    } else {
        GLint samplers[2] = {0, 1};
        glUseProgram(_tint_shaders.program);
        glUniform1iv(_tint_shaders.uniforms->sampler, 2, samplers);
        glUniform2f(_tint_shaders.uniforms->resolution, static_cast<float>(_resolution.x), static_cast<float>(_resolution.y));
    }
    if (!loadProgram(&_texture_shaders, texture_vsh(), texture_fsh())) {
        loge << "Failed to load \"texture\" shaders" << endl;
    } else {
        glUseProgram(_texture_shaders.program);
        glUniformMatrix4fv(_texture_shaders.uniforms->mvp_mat, 1, 0, _modelview_projection_matrix.m);
        glUniform1i(_texture_shaders.uniforms->sampler, 1);
    }
    if (!loadProgram(&_mask_shaders, mask_vsh(), texture_fsh())) {
        loge << "Failed to load \"mask\" shaders" << endl;
    } else {
        glUseProgram(_mask_shaders.program);
        glUniform1i(_mask_shaders.uniforms->sampler, 1);
    }
    if (!loadProgram(&_blur_shaders, texture_vsh(), blur_fsh(_scale_factor))) {
        loge << "Failed to load \"blur\" shaders" << endl;
    } else {
        glUseProgram(_blur_shaders.program);
        glUniformMatrix4fv(_blur_shaders.uniforms->mvp_mat, 1, 0, _modelview_projection_matrix.m);
        glUniform1i(_blur_shaders.uniforms->sampler, 1);
    }
    if (!loadProgram(&_overlay_shaders, overlay_vsh(), overlay_fsh())) {
        loge << "Failed to load \"overlay\" shaders" << endl;
    } else {
        glUseProgram(_overlay_shaders.program);
        glUniformMatrix4fv(_overlay_shaders.uniforms->mvp_mat, 1, 0, _modelview_projection_matrix.m);
        glUniform1i(_overlay_shaders.uniforms->sampler, 1);
    }
    if (!loadProgram(&_reduce_overlay_shaders, texture_vsh(), reduce_overlay_fsh())) {
        loge << "Failed to load \"reduce_overlay\" shaders" << endl;
    } else {
        glUseProgram(_reduce_overlay_shaders.program);
        glUniformMatrix4fv(_reduce_overlay_shaders.uniforms->mvp_mat, 1, 0, _modelview_projection_matrix.m);
        glUniform1i(_reduce_overlay_shaders.uniforms->sampler, 1);
    }
    if (!loadProgram(&_chip_shaders, chip_vsh(), chip_fsh())) {
        loge << "Failed to load \"chip\" shaders" << endl;
    } else {
        glUseProgram(_chip_shaders.program);
        glUniform3f(_chip_shaders.uniforms->light_pos, _light_position.x, _light_position.y, _light_position.z);
        glUniform1i(_chip_shaders.uniforms->sampler, 0);
    }
}

bool GLRenderer::loadProgram(IGLShaders *shader, const string &vsh, const string &fsh) {
    GLuint vertex_shader, fragment_shader;
    if (!compileShader(&vertex_shader, GL_VERTEX_SHADER, vsh)) {
        loge << "Failed to compile vertex shader" << endl;
        return false;
    }
    if (!compileShader(&fragment_shader, GL_FRAGMENT_SHADER, fsh)) {
        loge << "Failed to compile fragment shader" << endl;
        return false;
    }

    shader->program = glCreateProgram();
    glAttachShader(shader->program, vertex_shader);
    glAttachShader(shader->program, fragment_shader);
    glLinkProgram(shader->program);

    if (!getShaderStatus(shader->program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog)) {
        loge << "Failed to link program: " << shader->program << endl;

        if (vertex_shader) {
            glDeleteShader(vertex_shader);
        }
        if (fragment_shader) {
            glDeleteShader(fragment_shader);
        }
        if (shader->program) {
            glDeleteProgram(shader->program);
            shader->program = 0;
        }
        return false;
    }

    shader->getUniforms();

    if (vertex_shader) {
        glDetachShader(shader->program, vertex_shader);
        glDeleteShader(vertex_shader);
    }
    if (fragment_shader) {
        glDetachShader(shader->program, fragment_shader);
        glDeleteShader(fragment_shader);
    }

    glValidateProgram(shader->program);
    return getShaderStatus(shader->program, GL_VALIDATE_STATUS, glGetProgramiv, glGetProgramInfoLog);
}

bool GLRenderer::compileShader(GLuint *shader, GLenum type, const string &str) {
    const GLchar *source = str.c_str();
    if (!source) {
        loge << "Failed to load shader" << endl;
        return false;
    }
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    if (!getShaderStatus(*shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog)) {
        glDeleteShader(*shader);
        return false;
    }
    return true;
}

GLint GLRenderer::getShaderStatus(GLuint id, GLenum status, void(*attrib_func)(GLuint, GLenum, GLint *), void(*info_func)(GLuint, GLsizei, GLsizei *, GLchar *)) {
#ifdef DEBUG
    GLint length = 0;
    attrib_func(id, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
        GLchar *log = (GLchar *)malloc(length);
        info_func(id, length, &length, log);
        logi << __FUNCTION__ << " log:\n" << log << endl;
        free(log);
    }
#endif
    GLint param;
    attrib_func(id, status, &param);
    return param;
}

template<typename T>
void setVertexAttribPointer(GLuint index, GLuint ptr, GLsizei stride) {
    const auto size = sizeof(T) / sizeof(float);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(ptr));
}

template<typename T>
void setInstancedVertexAttribPointer(GLuint &index, GLuint &ptr, GLsizei stride) {
    setVertexAttribPointer<T>(index, ptr, stride);
    glVertexAttribDivisor(index, 1);
    index++;
    ptr += sizeof(T);
}

template<>
void setInstancedVertexAttribPointer<mat4>(GLuint &index, GLuint &ptr, GLsizei stride) {
    for (unsigned short i = 0; i < 4; ++i) {
        setInstancedVertexAttribPointer<vec4<float>>(index, ptr, stride);
    }
}

template<>
void setInstancedVertexAttribPointer<mat3>(GLuint &index, GLuint &ptr, GLsizei stride) {
    for (unsigned short i = 0; i < 3; ++i) {
        setInstancedVertexAttribPointer<vec3<float>>(index, ptr, stride);
    }
}

template<typename ... T>
struct VertexAttribPointerConstructor;

template<typename T0, typename ... T>
struct VertexAttribPointerConstructor<T0, T...> {
    static GLuint set(GLuint index, GLuint ptr, GLsizei stride) {
        setVertexAttribPointer<T0>(index, ptr, stride);
        return VertexAttribPointerConstructor<T...>::set(index + 1, ptr + sizeof(T0), stride);
    }

    static GLuint setInstanced(GLuint index, GLuint ptr, GLsizei stride) {
        setInstancedVertexAttribPointer<T0>(index, ptr, stride);
        return VertexAttribPointerConstructor<T...>::setInstanced(index, ptr, stride);
    }
};

template<>
struct VertexAttribPointerConstructor<> {
    static GLuint set(GLuint index, GLuint ptr, GLsizei stride) {
        return index;
    }

    static GLuint setInstanced(GLuint index, GLuint ptr, GLsizei stride) {
        return index;
    }
};

template<typename ... T>
GLuint GLRenderer::setUpBuffer(GLModel *model, const GLVertex<T...> *data, GLsizeiptr size, GLenum usage) {
    glGenVertexArrays(1, &model->vertex_array);
    glBindVertexArray(model->vertex_array);

    glGenBuffers(1, &model->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, model->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);

    return VertexAttribPointerConstructor<T...>::set(0, 0, sizeof(*data));
}

template<typename ... T>
void GLRenderer::setUpInstanceBuffer(GLInstancedModel *model, const GLVertex<T...> *data, GLsizeiptr size, GLuint index) {
    glBindVertexArray(model->vertex_array);

    glGenBuffers(1, &model->instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, model->instance_buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

    VertexAttribPointerConstructor<T...>::setInstanced(index, 0, sizeof(*data));
}

void GLRenderer::setUpBuffer(GLModel *model, const unsigned short *data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &model->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

void GLRenderer::setUpTexture(GLuint *texture, GLsizei width, GLsizei height, void *data) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void GLRenderer::updateTintShader(const rect<float> &tex_offset, const vec3<float> &label_data, const mat4 &mvp_mat) {
    get<0>(_tint_shader_instances[_tint_shader_current_instance_count]).x = tex_offset.origin.x;
    get<0>(_tint_shader_instances[_tint_shader_current_instance_count]).y = tex_offset.origin.y;
    get<0>(_tint_shader_instances[_tint_shader_current_instance_count]).z = tex_offset.size.x;
    get<0>(_tint_shader_instances[_tint_shader_current_instance_count]).w = tex_offset.size.y;
    get<1>(_tint_shader_instances[_tint_shader_current_instance_count]) = label_data;
    get<2>(_tint_shader_instances[_tint_shader_current_instance_count]) = mvp_mat;
    _tint_shader_current_instance_count++;
}

void GLRenderer::deleteModel(const GLModel &model) {
    if (model.index_buffer) {
        glDeleteBuffers(1, &model.index_buffer);
    }
    if (model.vertex_buffer) {
        glDeleteBuffers(1, &model.vertex_buffer);
    }
    if (model.vertex_array) {
        glDeleteVertexArrays(1, &model.vertex_array);
    }
}

void GLRenderer::deleteShaders(const IGLShaders &shaders) {
    if (shaders.program) {
        glDeleteProgram(shaders.program);
    }
}

void GLRenderer::clearFramebufferTexture(GLuint texture) {
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT);
}
