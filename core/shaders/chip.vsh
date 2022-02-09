GLSL3(
uniform mat4 proj_mat;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 tex_offset_0;
layout(location = 4) in vec4 tex_offset_1;
layout(location = 5) in mat4 mv_mat;
layout(location = 9) in mat3 normal_mat;
layout(location = 12) in float data;

out vec3 position_data;
out vec3 normal_data;
out vec2 tex_coord_data[2];
out vec2 tex_offset_data;
flat out int level_data;

void main() {
    level_data = int(data + 0.5);
    vec4 p = mv_mat * position;
    position_data = p.xyz / p.w;
    normal_data = normalize(normal_mat * normal);
    tex_offset_data = tex_coord * 2.0 - 0.5;
    tex_coord_data[0] = tex_offset_0.xy + tex_coord * tex_offset_0.zw;
    tex_coord_data[1] = tex_offset_1.xy + tex_offset_data * tex_offset_1.zw;
    gl_Position = proj_mat * mv_mat * position;
}
)
