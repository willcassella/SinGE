// LightmaskVolume.h
#pragma once

#include "glew.h"

namespace sge
{
	namespace gl_render
	{
		static constexpr uint32 NUM_FRUSTUM_ELEMS = 36;

		/**
		 * \brief Fills the given element buffer with vertex elements for a frustum. This only needs to be called once.
		 * \param ebo The element buffer to fill.
		 */
		void create_lightmask_volume_frustm_elems(
			GLuint ebo);

		/**
		 * \brief Fills the given vertex position buffer with vertex coordinates for the given frustum.
		 * \param pos_buffer The vertex position buffer to fill.
		 * \param near_plane The distance from the frustum center to the near plane.
		 * \param far_plane The distance from the frustum center to the far plane.
		 * \param horiz_angle_radians The angle between the left and right planes of the frustum, in radians.
		 * \param vert_angle_radians The angle between the top and bottom planes of the frustum, in radians.
		 */
		void create_lightmask_volume_frustum_positions(
			GLuint pos_buffer,
			float near_plane,
			float far_plane,
			float horiz_angle_radians,
			float vert_angle_radians);
	}
}
