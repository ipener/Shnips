GLSL3(
uniform mat4 mvp_mat;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

out lowp vec4 gradient_data;

void main() {
    gradient_data = color;
    gl_Position = mvp_mat * position;
}
)
