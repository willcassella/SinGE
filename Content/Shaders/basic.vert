// basic.vert
#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 v_position;
in vec3 v_normal;
in vec3 v_tangent;
in int v_bitangent_sign;
in vec2 v_mat_texcoord;
in vec2 v_lm_texcoord;

out VS_OUT {
	vec2 mat_tex_coords;
	vec2 lm_tex_coords;
	vec3 cam_position;
	vec3 cam_tangent;
	vec3 cam_bitangent;
	vec3 cam_normal;
} vs_out;

void main()
{
	// Output texture coordinates
	vs_out.mat_tex_coords = v_mat_texcoord;
	vs_out.lm_tex_coords = v_lm_texcoord;

	// Compute screen-space position, and view-space position
	gl_Position = projection * view * model * vec4(v_position, 1);
	vs_out.cam_position = (view * model * vec4(v_position, 1)).xyz;

	// Compute tangent and normal in camera space
	mat4 model_view = transpose(inverse(view * model));
	vec3 tangent = normalize(vec3(model_view *	vec4(v_tangent,   0)));
	vec3 normal = normalize(vec3(model_view *	vec4(v_normal,    0)));

	// Re-orthogonalize tangent with respect to normal
	tangent = normalize(tangent - dot(tangent, normal) * normal);

	// Output TBN
	vs_out.cam_tangent = tangent;
	vs_out.cam_bitangent = v_bitangent_sign * cross(normal, tangent);
	vs_out.cam_normal = normal;
}
