GLSL3(
uniform mat4 mvp_mat;
uniform vec4 data;

layout(location = 0) in vec4 position;

out mediump vec2 tex_coord_data;

void main() {
    tex_coord_data = (position.xy + data.xy) / data.zw;
    gl_Position = mvp_mat * position;
}
)
