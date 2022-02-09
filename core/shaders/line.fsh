GLSL3(
precision highp float;

uniform lowp vec4 color;
uniform float data;

in vec2 position_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    vec2 p = vec2(0.5, 0.0);
    vec2 q = vec2(data - 0.5, 0.0);
    vec2 position = vec2((position_data.x + 0.5) * data, position_data.y);
    if (position.x < p.x) {
        float d = distance(position, p);
        if (d < 0.5 - fwidth(d)) {
            color_data.a = color.a;
        } else {
            if (p.x - position.x < 0.25) {
                color_data.a = (1.0 - smoothstep(0.5 - abs(dFdy(d)), 0.5, d)) * color.a;
            } else {
                color_data.a = (1.0 - smoothstep(0.5 - abs(dFdx(d)), 0.5, d)) * color.a;
            }
        }
        color_data.rgb = color.rgb;
    } else if (position.x > q.x) {
        float d = distance(position, q);
        if (d < 0.5 - fwidth(d)) {
            color_data.a = color.a;
        } else {
            if (position.x - q.x < 0.25) {
                color_data.a = (1.0 - smoothstep(0.5 - abs(dFdy(d)), 0.5, d)) * color.a;
            } else {
                color_data.a = (1.0 - smoothstep(0.5 - abs(dFdx(d)), 0.5, d)) * color.a;
            }
        }
        color_data.rgb = color.rgb;
    } else {
        color_data = color;
    }
}
)
