#pragma once

#include <stdint.h>

#include "lib/base/math/vec3.h"
#include "lib/resource/build.h"

namespace sge
{
    static constexpr uint32_t NUM_FRUSTUM_ELEMS = 36;
    static constexpr uint32_t NUM_FRUSTUM_VERTS = 24;

    /**
        * \brief Fills the given element buffer with vertex elements for a frustum.
        * \param out_elems The element buffer to fill. This must have NUM_FRUSTUM_ELEMS elements.
        */
    SGE_RESOURCE_API void create_lightmask_volume_frustum_elems(
        uint32_t* out_elems);

    SGE_RESOURCE_API void create_lightmask_volume_frustum_normals(
        float horiz_angle_radians,
        float vert_angle_radians,
        Vec3* out_normals);

    SGE_RESOURCE_API void create_lightmask_volume_frustum_texcoords(
        float near_plane,
        float far_plane,
        float horiz_angle_radians,
        float vert_angle_radians,
        Vec2* out_texcoords);

    /**
        * \brief Fills the given vertex position buffer with vertex coordinates for the given frustum.
        * \param near_plane The distance from the frustum center to the near plane.
        * \param far_plane The distance from the frustum center to the far plane.
        * \param horiz_angle_radians The angle between the left and right planes of the frustum, in radians.
        * \param vert_angle_radians The angle between the top and bottom planes of the frustum, in radians.
        * \param out_positions The buffer to fill. This must have NUM_FRUSTUM_VERTS elements.
        */
    SGE_RESOURCE_API void create_lightmask_volume_frustum_positions(
        float near_plane,
        float far_plane,
        float horiz_angle_radians,
        float vert_angle_radians,
        Vec3* out_positions);
}
