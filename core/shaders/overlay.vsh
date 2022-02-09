GLSL3(
uniform mat4 mvp_mat;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 tex_coord;

out mediump vec3 tex_coord_data;

void main() {
    tex_coord_data = tex_coord;
    gl_Position = mvp_mat * position;
}
)
