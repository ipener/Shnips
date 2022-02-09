GLSL3(
precision highp float;

uniform lowp vec4 color;
uniform vec3 data;

in vec2 position_data;

layout(location = 0) out lowp vec4 color_data;

const float M_PI = 3.14159;

void main() {
    float d = length(position_data);
    float w = data.x / 2.0;
    float fw = fwidth(d);
    if (d < 1.0 - w - fw || d > 1.0 + w + fw) {
        discard;
    } else if (d < 1.0) {
        float a = position_data.x < 0.0 ? 2.0 * M_PI - acos(position_data.y / d) : acos(position_data.y / d);
        float r = 1.0 - w;
        if (a < data.y) {
            color_data.a = smoothstep(r - fw, r, d) * color.a;
        } else {
            color_data.a = smoothstep(r - fw, r, d) * color.a * data.z;
        }
    } else {
        float a = position_data.x < 0.0 ? 2.0 * M_PI - acos(position_data.y / d) : acos(position_data.y / d);
        float r = 1.0 + w;
        if (a < data.y) {
            color_data.a = (1.0 - smoothstep(r, r + fw, d)) * color.a;
        } else {
            color_data.a = (1.0 - smoothstep(r, r + fw, d)) * color.a * data.z;
        }
    }
    color_data.rgb = color.rgb;
}
)
