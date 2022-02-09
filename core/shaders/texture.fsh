GLSL3(
uniform lowp sampler2D sampler;

in mediump vec2 tex_coord_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    color_data = texture(sampler, tex_coord_data);
}
)
