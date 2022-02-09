GLSL3(
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 tex_offset;
layout(location = 3) in vec3 label_data;
layout(location = 4) in mat4 mvp_mat;

out mediump vec2 tex_coord_data;
flat out mediump vec3 data;

void main() {
    data = label_data;
    tex_coord_data = tex_offset.xy + tex_coord * tex_offset.zw;
    gl_Position = mvp_mat * position;
}
)
