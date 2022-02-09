GLSL3(
uniform lowp sampler2D sampler[2];
uniform highp vec2 resolution;

in mediump vec2 tex_coord_data;
flat in mediump vec3 data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    lowp vec3 color = vec3(0.0, 0.0, 0.0);
    if (data.x > 0.0) {
        color = texture(sampler[1], gl_FragCoord.xy / resolution).rgb;
    }
    lowp int index = int(data.z + 0.5);
    color_data = vec4(color, texture(sampler[0], tex_coord_data)[index] * data.y);
}
)
