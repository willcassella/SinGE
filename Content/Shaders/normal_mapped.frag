// normal_mapped.frag
#version 430 core

const vec3 lm_x_basis_vec = vec3(0.81649, 0, 0.57735);
const vec3 lm_y_basis_vec = vec3(-0.40824, 0.707106, 0.57735);
const vec3 lm_z_basis_vec = vec3(-0.48024, -0.707106, 0.57735);

uniform sampler2D lightmap_x_basis;
uniform sampler2D lightmap_y_basis;
uniform sampler2D lightmap_z_basis;
uniform sampler2D lightmap_direct_mask;
uniform sampler2D ao_map;
uniform sampler2D albedo;
uniform sampler2D normal_map;
uniform sampler2D roughness_map;
uniform sampler2D metallic_map;
uniform float roughness_constant = 0.5;
uniform float metallic_constant = 0;
uniform bool use_lightmap = false;
uniform bool use_ao_map = false;
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
    // Compute TBN matrix (converts tangent space to camera space)
    mat3 TBN = mat3(
        normalize(fs_in.cam_tangent),
        normalize(fs_in.cam_bitangent),
        normalize(fs_in.cam_normal));

    // Transform normal sample to range [-1, 1]
    vec3 normal = texture(normal_map, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale).xyz;
    normal = normalize(normal * 2.0f - 1.0f);

    // Output position, normal, and albedo
    out_position = fs_in.cam_position;
    out_normal = normalize(TBN * normal);

    // Output albedo
    const vec4 albedo = texture(albedo, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale);
    out_albedo = albedo;

    // Get AO
    float ao = 1.0f;
    if (use_ao_map)
    {
        ao = texture(ao_map, fs_in.mat_tex_coords * base_mat_uv_scale * inst_mat_uv_scale).r;
    }

    // Output irradiance
    if (use_lightmap)
    {
        vec3 irradiance = vec3(0.f);
        irradiance += texture(lightmap_x_basis, fs_in.lm_tex_coords).rgb * max(dot(lm_x_basis_vec, normal), 0.f);
        irradiance += texture(lightmap_y_basis, fs_in.lm_tex_coords).rgb * max(dot(lm_y_basis_vec, normal), 0.f);
        irradiance += texture(lightmap_z_basis, fs_in.lm_tex_coords).rgb * max(dot(lm_z_basis_vec, normal), 0.f);
        out_irradiance = vec4(ao * irradiance * albedo.rgb, texture(lightmap_direct_mask, fs_in.lm_tex_coords).r);
    }
    else
    {
        out_irradiance = vec4(0, 0, 0, 1);
    }

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
