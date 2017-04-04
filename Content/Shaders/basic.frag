// basic.frag
#version 430 core

uniform sampler2D albedo;
uniform sampler2D lightmap;
uniform sampler2D roughness_map;
uniform sampler2D metallic_map;
uniform float roughness_constant = 0.5;
uniform float metallic_constant = 0;
uniform bool use_roughness_map = false;
uniform bool use_metallic_map = false;
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

    // Output albedo
    out_albedo = texture(albedo, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale);

    // Output irradiance
    out_irradiance = texture(lightmap, fs_in.lm_tex_coords);

    // Output roughness
    if (use_roughness_map)
    {
        out_roughness_metallic.r = texture(roughness_map, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale).r;
    }
    else
    {
        out_roughness_metallic.r = roughness_constant;
    }

    // Output metallic
    if (use_metallic_map)
    {
        out_roughness_metallic.g = texture(metallic_map, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale).r;
    }
    else
    {
        out_roughness_metallic.g = metallic_constant;
    }
}
