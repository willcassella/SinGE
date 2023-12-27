#version 410 core

uniform sampler2D depth_buffer;
uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;
uniform sampler2D roughness_metallic_buffer;
uniform sampler2D hdr_buffer;
uniform float gamma = 2.2;
uniform float brightness_boost = 0.0;

in vec2 f_texcoord;

out vec4 out_color;

vec3 brightness_contrast(vec3 value, float brightness, float contrast)
{
    return (value - 0.5f) * contrast + 0.5f + brightness;
}

vec3 reinhard_tone_mapping(vec3 value)
{
    return value / (value + vec3(1.0f));
}

vec3 rgb_to_hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec3 scene_color = texture(hdr_buffer, f_texcoord).rgb;

    // Increase contrast
    scene_color = brightness_contrast(scene_color, 0.0, 1.01f);

    // Do tone mapping (Reinhard operator)
    scene_color = reinhard_tone_mapping(scene_color);

    // Do brightness blending
    scene_color.r += brightness_boost * pow(1.f - scene_color.r, 1.f/2);
    scene_color.g += brightness_boost * pow(1.f - scene_color.g, 1.f/2);
    scene_color.b += brightness_boost * pow(1.f - scene_color.b, 1.f/2);

    // Increase saturation
    vec3 scene_hsv = rgb_to_hsv(scene_color);
    scene_hsv.g += scene_hsv.g * 0.1f;
    scene_color = hsv_to_rgb(scene_hsv);

    // Do gamma correction
    scene_color.r = pow(scene_color.r, 1.f/gamma);
    scene_color.g = pow(scene_color.g, 1.f/gamma);
    scene_color.b = pow(scene_color.b, 1.f/gamma);
    out_color = vec4(scene_color, 1.f);
}
