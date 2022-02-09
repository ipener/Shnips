GLSL3(
uniform lowp sampler2D sampler;
uniform highp float data;

in mediump vec2 tex_coord_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    mediump float c = texture(sampler, tex_coord_data).a;
    if (c + data < 0.5) {
        c += data;
    } else if (c - data > 0.5){
        c -= data;
    } else {
        c = 0.5;
    }
    color_data.a = c;
}
)
