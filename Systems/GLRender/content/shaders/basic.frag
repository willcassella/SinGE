// default.frag
#version 430 core

uniform sampler2D diffuse;

in vec3 f_position;
in vec3 f_normal;
in vec2 f_texcoord;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec4 outDiffuse;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outSpecular;

void main()
{
    outDiffuse = vec4(1, 1, 0, 1);
    //outDiffuse = texture(diffuse, f_coord);
}
