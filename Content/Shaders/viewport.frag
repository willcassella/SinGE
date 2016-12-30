// viewport.frag
#version 330 core

uniform sampler2D depth_buffer;
uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D diffuse_buffer;
uniform sampler2D specular_buffer;

in vec2 f_texcoord;

out vec4 out_color;

void main()
{
    out_color = texture(diffuse_buffer, f_texcoord);
}
