GLSL3(
uniform mat4 mvp_mat;

layout(location = 0) in vec4 position;

void main() {
    gl_Position = mvp_mat * position;
}
)
