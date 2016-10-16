// default.vert
#version 430 core

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec3 f_position;
out vec3 f_normal;
out vec2 f_texcoord;

void main()
{
	gl_Position = projection * view * model * vec4(v_position, 1);
	f_position = (model * vec4(v_position, 1)).xyz;
	f_normal = (transpose(inverse(model)) * vec4(v_normal, 0)).xyz;
	f_texcoord = v_texcoord;
}