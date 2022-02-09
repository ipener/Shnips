GLSL3(
uniform mat4 mvp_mat;

layout(location = 0) in vec4 position;

out highp vec2 position_data;

void main() {
    position_data = position.xy;
    gl_Position = mvp_mat * position;
}
)
