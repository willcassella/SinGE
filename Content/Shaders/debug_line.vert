// line.vert
#version 330 core

uniform mat4 view;
uniform mat4 projection;

in vec3 v_position;
in vec3 v_color;

out vec3 f_color;

void main()
{
	gl_Position = projection * view * vec4(v_position, 1);
	f_color = v_color;
}
