// viewport.vert
#version 330 core

in vec2 v_position;
in vec2 v_mat_texcoord;

out vec2 f_texcoord;

void main()
{
	gl_Position = vec4(v_position, 0, 1);
	f_texcoord = v_mat_texcoord;
}