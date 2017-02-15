// hdr_post.frag
#version 430 core

uniform sampler2D depth_buffer;
uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;
uniform sampler2D roughness_metallic_buffer;
uniform sampler2D hdr_buffer;

in vec2 f_texcoord;

out vec4 out_color;

void main()
{
    vec3 scene_color = texture(hdr_buffer, f_texcoord).rgb;

    // Do tone mapping (Reinhard operator)
    out_color = vec4(scene_color / (scene_color + vec3(1.0f)), 1);
}
