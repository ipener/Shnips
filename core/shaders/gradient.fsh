GLSL3(
in lowp vec4 gradient_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    color_data = gradient_data;
}
)
