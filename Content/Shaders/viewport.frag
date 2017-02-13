// viewport.frag
#version 430 core

const float PI = 3.141592654f;
const vec3 F0 = vec3(0.04f);

uniform mat4 view;

uniform sampler2D depth_buffer;
uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;
uniform sampler2D roughness_metallic_buffer;

in vec2 f_texcoord;

out vec4 out_color;

// Compute k constant for direct lighting
float geom_k_direct(float alpha)
{
    return (alpha + 1.0f) * (alpha + 1.0f) / 8;
}

// Implements the schlick-ggx Geometry Masking function
float schlick_ggx_GMF(float n_dot, float k)
{
    float denom = n_dot * (1.0f - k) + k;
    return n_dot/denom;
}

float smith_GMF(vec3 N, vec3 V, vec3 L, float k)
{
    float n_dot_v = max(dot(N, V), 0.0f);
    float n_dot_l = max(dot(N, L), 0.0f);
    return schlick_ggx_GMF(n_dot_v, k) * schlick_ggx_GMF(n_dot_l, k);
}

// Computes the normal distribution function
float ggx_tr_NDF(vec3 N, vec3 H, float alpha)
{
    float a2 = alpha * alpha;
    float n_dot_h_2 = max(dot(N, H), 0.0f);
    n_dot_h_2 = n_dot_h_2 * n_dot_h_2;

    float denom = n_dot_h_2 * (a2 - 1.0f) + 1.0f;
    denom = PI * denom * denom;
    return a2 / denom;
}

// Calculate base reflectivity
vec3 base_reflectivity(vec3 albedo, float metallic)
{
    return mix(F0, albedo, metallic);
}

vec3 fresnel_schlick_roughness(float cos_theta, vec3 base_ref, float roughness)
{
    return base_ref + (max(vec3(1.0f - roughness), base_ref) - base_ref) * pow(1.0f - cos_theta, 5);
}

void main()
{
    // Get a light vector in camera-space normal
    mat4 normal_to_view = transpose(inverse(view));
    vec3 light = normalize((normal_to_view * vec4(0.0f, 0.5f, 0.5f, 0.0f)).xyz);

    vec4 albedo = texture(albedo_buffer, f_texcoord);
    vec3 cam_pos = texture(position_buffer, f_texcoord).xyz;
    vec3 cam_norm = texture(normal_buffer, f_texcoord).xyz;
    float roughness = texture(roughness_metallic_buffer, f_texcoord).r;
    float metallic = texture(roughness_metallic_buffer, f_texcoord).g;

    vec3 view = normalize(-cam_pos);

    // Calculate the halfway vector
    vec3 half_vec = normalize(view + light);

    // Calculate (n dot v) and (n dot l)
    float n_dot_v = max(dot(cam_norm, view), 0.0f);
    float n_dot_l = max(dot(cam_norm, light), 0.0f);

    // Calculate NDF
    float ndf_alpha = roughness * roughness;
    float ndf = ggx_tr_NDF(cam_norm, half_vec, ndf_alpha);

    // Calculate the geometric masking constant for direct lighting
    float geom_k = geom_k_direct(roughness);

    // Calculate GMF
    float gmf = smith_GMF(cam_norm, view, light, geom_k);

    // Calculate base reflectivity
    vec3 base_ref = base_reflectivity(albedo.xyz, metallic);

    // Calculate fresnel
    vec3 fresnel = fresnel_schlick_roughness(n_dot_v, base_ref, roughness);

    // Calculate ratio of refraction
    vec3 kD = vec3(1.0f) - fresnel;
    kD *= 1.0f - metallic;

    // Do shading
    vec3 Lo = vec3(0.0f);

    // For each light
        vec3 light_color = vec3(23.47f, 21.31f, 20.79f);
        float attenutation = 1.0f;
        vec3 radiance = light_color * attenutation;

        // Calculate BRDF
        vec3 brdf_nom = ndf * gmf * fresnel;
        float brdf_denom = 4 * n_dot_v * n_dot_l + 0.001f;
        vec3 brdf = brdf_nom / brdf_denom;
        Lo += (kD * albedo.xyz / PI + brdf) * radiance * n_dot_l;
    // end for

    vec3 ambient = vec3(0.046, 0.054, 0.064) * albedo.xyz / 2;
    vec3 color = ambient + Lo;

    // Do tone mapping
    color = color / (color + vec3(1.0f));

    out_color = vec4(color, 1.0f);
}
