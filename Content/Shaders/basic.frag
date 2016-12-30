// default.frag
#version 430 core

uniform sampler2D diffuse;

in vec3 f_position;
in vec3 f_normal;
in vec2 f_texcoord;

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out float out_specular;

void main()
{
    out_position = f_position;
    out_normal = f_normal;
    out_diffuse = texture(diffuse, f_texcoord);
    out_specular = 0.3;
}
