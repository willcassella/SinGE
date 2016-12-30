// line.frag
#version 430 core

in vec3 fColor;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec4 outDiffuse;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec4 outSpecular;

void main()
{
    outDiffuse = vec4(fColor, 1);
}
