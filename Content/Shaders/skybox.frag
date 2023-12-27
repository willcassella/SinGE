#version 410 core

uniform sampler2D skybox;
uniform vec2 base_mat_uv_scale = vec2(1, 1);
uniform vec2 inst_mat_uv_scale = vec2(1, 1);

in VS_OUT {
    vec2 mat_tex_coords;
    vec2 lm_tex_coords;
    vec3 cam_position;
    vec3 cam_tangent;
    vec3 cam_bitangent;
    vec3 cam_normal;
} fs_in;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_albedo;
layout (location = 3) out vec2 out_roughness_metallic;
layout (location = 4) out vec4 out_irradiance;

void main()
{
    out_position = fs_in.cam_position;
    out_normal = normalize(fs_in.cam_normal);
    out_albedo = vec4(0, 0, 0, 1.f);
    out_roughness_metallic = vec2(0, 0);

    // Output irradiance
    vec3 skybox_color = texture(skybox, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale).rgb;
    out_irradiance = vec4(skybox_color, 0);
}
