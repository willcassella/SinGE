// line.vert
#version 430 core

layout(location = 0) uniform mat4 view;
layout(location = 1) uniform mat4 projection;

in vec3 vPosition;
in vec3 vColor;

out vec3 fColor;

void main()
{
	gl_Position = projection * view * vec4(vPosition, 1);
	fColor = vColor;
}