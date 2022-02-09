GLSL3(
uniform lowp sampler2D sampler;
uniform mediump vec3 light_pos;

in mediump vec3 position_data;
in mediump vec3 normal_data;
in mediump vec2 tex_coord_data[2];
in mediump vec2 tex_offset_data;
flat in lowp int level_data;

layout(location = 0) out lowp vec4 color_data;

void main() {
    mediump float diffuse = max(dot(normalize(light_pos - position_data), normal_data), 0.0) * 0.1;
    lowp float c;
    if (tex_offset_data.x < 0.01 || tex_offset_data.y < 0.01 || tex_offset_data.x > 0.99 || tex_offset_data.y > 0.99) {
        c = 0.0;
    } else {
        c = texture(sampler, tex_coord_data[1])[level_data];
    }
    color_data.rgb = (1.0 - c) * texture(sampler, tex_coord_data[0]).rgb * 0.9 + diffuse;
    color_data.a = 1.0;
}
)
