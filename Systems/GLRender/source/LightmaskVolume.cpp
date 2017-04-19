// LightmaskVolume.cpp

#include <cmath>
#include <Core/env.h>
#include "../private/LightmaskVolume.h"

namespace sge
{
	namespace gl_render
	{
		void create_lightmask_volume_frustm_elems(
			GLuint ebo)
		{
			const uint32 elems[NUM_FRUSTUM_ELEMS] = {
				// Near face
				2, 8, 5,
				2, 11, 8,

				// Left face
				12, 3, 21,
				12, 0, 3,

				// Bottom face
				13, 10, 1,
				13, 16, 10,

				// Right face
				9, 18, 6,
				9, 15, 18,

				// Top face
				4, 19, 22,
				4, 7, 19,

				// Far face
				17, 23, 20,
				17, 14, 23
			};

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
		}

		void create_lightmask_volume_frustum_positions(
			GLuint pos_buffer,
			float near_plane,
			float far_plane,
			float horiz_angle_radians,
			float vert_angle_radians)
		{
			// 8 vertices, 3 instances of each, 3 components in each instance
			float buff[8 * 3 * 3];
			const float sin_half_horiz_angle = sinf(horiz_angle_radians * 0.5f);
			const float sin_half_vert_angle = sinf(vert_angle_radians * 0.5f);

			// Near bottom left vertex (occupies indices 0, 1, 2)
			buff[0 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[0 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[0 * 3 + 2] = -near_plane;
			buff[1 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[1 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[1 * 3 + 2] = -near_plane;
			buff[2 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[2 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[2 * 3 + 2] = -near_plane;

			// Near top left vertex (occupies indices 3, 4, 5)
			buff[3 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[3 * 3 + 1] = near_plane * sin_half_vert_angle;
			buff[3 * 3 + 2] = -near_plane;
			buff[4 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[4 * 3 + 1] = near_plane * sin_half_vert_angle;
			buff[4 * 3 + 2] = -near_plane;
			buff[5 * 3 + 0] = near_plane * -sin_half_horiz_angle;
			buff[5 * 3 + 1] = near_plane * sin_half_vert_angle;
			buff[5 * 3 + 2] = -near_plane;

			// Near top right vertex (occupies indices 6, 7, 8)
			buff[6 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[6 * 3 + 1] = near_plane * sin_half_horiz_angle;
			buff[6 * 3 + 2] = -near_plane;
			buff[7 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[7 * 3 + 1] = near_plane * sin_half_horiz_angle;
			buff[7 * 3 + 2] = -near_plane;
			buff[8 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[8 * 3 + 1] = near_plane * sin_half_horiz_angle;
			buff[8 * 3 + 2] = -near_plane;

			// Near bottom right vertex (occupies indices 9, 10, 11)
			buff[9 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[9 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[9 * 3 + 2] = -near_plane;
			buff[10 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[10 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[10 * 3 + 2] = -near_plane;
			buff[11 * 3 + 0] = near_plane * sin_half_horiz_angle;
			buff[11 * 3 + 1] = near_plane * -sin_half_vert_angle;
			buff[11 * 3 + 2] = -near_plane;

			// Far bottom left vertex (occupies indices 12, 13, 14)
			buff[12 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[12 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[12 * 3 + 2] = -far_plane;
			buff[13 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[13 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[13 * 3 + 2] = -far_plane;
			buff[14 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[14 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[14 * 3 + 2] = -far_plane;

			// Far bottom right vertex (occupies indices 15, 16, 17)
			buff[15 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[15 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[15 * 3 + 2] = -far_plane;
			buff[16 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[16 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[16 * 3 + 2] = -far_plane;
			buff[17 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[17 * 3 + 1] = far_plane * -sin_half_horiz_angle;
			buff[17 * 3 + 2] = -far_plane;

			// Far top right vertex (occupieis indices 18, 19, 20)
			buff[18 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[18 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[18 * 3 + 2] = -far_plane;
			buff[19 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[19 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[19 * 3 + 2] = -far_plane;
			buff[20 * 3 + 0] = far_plane * sin_half_horiz_angle;
			buff[20 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[20 * 3 + 2] = -far_plane;

			// Far top left vertex (occupies indices 21, 22, 23)
			buff[21 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[21 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[21 * 3 + 2] = -far_plane;
			buff[22 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[22 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[22 * 3 + 2] = -far_plane;
			buff[23 * 3 + 0] = far_plane * -sin_half_horiz_angle;
			buff[23 * 3 + 1] = far_plane * sin_half_vert_angle;
			buff[23 * 3 + 2] = -far_plane;

			glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(buff), buff, GL_DYNAMIC_DRAW);
		}
	}
}
