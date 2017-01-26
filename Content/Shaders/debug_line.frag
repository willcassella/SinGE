// line.frag
#version 430 core

in vec3 f_color;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out float out_specular;

void main()
{
    out_diffuse = vec4(f_color, 1);
}
