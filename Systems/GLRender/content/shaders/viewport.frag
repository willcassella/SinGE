// viewport.frag
#version 330 core

uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D diffuse_buffer;
uniform sampler2D specular_buffer;

in vec2 f_texcoord;

out vec4 outColor;

void main()
{
    outColor = texture(diffuse_buffer, f_texcoord);
    //outColor = vec4(f_texcoord, 0, 1);
}
