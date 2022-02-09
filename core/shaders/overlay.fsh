GLSL3(
uniform lowp sampler2D sampler;
uniform mediump vec3 color[2];

in mediump vec3 tex_coord_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    mediump float a = texture(sampler, tex_coord_data.xy).a;
    mediump vec3 c0 = color[0] + (color[1] - color[0]) * tex_coord_data.z;

    if (c0.r < 0.5) {
        color_data.r = 2.0 * c0.r * a;
    } else {
        color_data.r = 1.0 - 2.0 * (1.0 - c0.r) * (1.0 - a);
    }
    if (c0.g < 0.5) {
        color_data.g = 2.0 * c0.g * a;
    } else {
        color_data.g = 1.0 - 2.0 * (1.0 - c0.g) * (1.0 - a);
    }
    if (c0.b < 0.5) {
        color_data.b = 2.0 * c0.b * a;
    } else {
        color_data.b = 1.0 - 2.0 * (1.0 - c0.b) * (1.0 - a);
    }
    color_data.a = 1.0;
}
)
