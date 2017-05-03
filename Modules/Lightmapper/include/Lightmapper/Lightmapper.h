// Lightmapper.h
#pragma once

#include <cmath>
#include <Core/Math/Mat4.h>
#include <Resource/Resources/StaticMesh.h>
#include "build.h"

namespace sge
{
    struct LightmapTexel
    {
        /**
         * \brief Matrix consisting of tangent, bitangent, and normal of the surface represented by this texel.
         */
        Mat4 TBN;

        /**
         * \brief The world position of the surface represented by this texel.
         */
        Vec4 world_pos;

		/*
		 * \breif The base color of the material represented by this texel.
		 */
		color::RGBF32 base_color;
    };

    struct LightmapObject
    {
        /**
         * \brief The mesh used by this object.
         */
        const StaticMesh* mesh = nullptr;

        /**
         * \brief The world transform of this object.
         */
        Mat4 world_transform;

	    /**
		 * \brief The base color of the material represented by this texel.
		 */
		color::RGBF32 base_color;
    };

    struct LightmapOccluder
    {
        /**
         * \brief The mesh used by this object.
         */
        const StaticMesh* mesh = nullptr;

        /**
         * \brief The world transform of this object.
         */
        Mat4 world_transform;

        /**
         * \brief Width (in pixels) of the irradiance map for this object.
         */
        int32 irradiance_width = 0;

        /**
         * \brief Height (in pixels) of the irradiance map for this object.
         */
        int32 irradiance_height = 0;

        /**
         * \brief A texture that describes the irradiance of this object.
         */
        const color::RGBF32* irradiance = nullptr;
    };

    struct LightmapLight
    {
        /**
         * \brief The direction this light is shining.
         */
        Vec3 direction;

        /**
         * \brief The color intensity of this light.
         */
        color::RGBF32 intensity;
    };

    struct LightmapScene;

	/**
	 * \brief Returns the tangent-space x basis vector for lightmapping.
	 */
	inline Vec3 get_lightmap_x_basis_vector()
	{
		return Vec3{ sqrtf(2.f / 3.f), 0.f, sqrtf(1.f / 3.f) };
	}

	/**
	 * \brief Returns the tangent-space y basis vector for lightmapping.
	 */
	inline Vec3 get_lightmap_y_basis_vector()
	{
		return Vec3{ -sqrtf(1.f / 6.f), sqrtf(1.f / 2.f), sqrtf(1.f / 3.f) };
	}

	/**
	 * \brief Returns the tangent-space z basis vector for lightmapping.
	 */
	inline Vec3 get_lightmap_z_basis_vector()
	{
		return Vec3{ -sqrtf(1.f / 6.f), -sqrtf(1.f / 2.f), sqrtf(1.f / 3.f) };
	}

    /**
     * \brief Generates lightmap texel information for the given lightmap objects.
     * \param objects An array of objects in the lightmap. All fields must be initialized.
     * \param num_objects The number of objects in the lightmap.
     * \param width The width (in pixels) of the lightmap.
     * \param height The hight (in pixels) of the lightmap.
     * \param out_texels The array of texels to be assigned to (must be width * height in size).
     * \param out_texel_mask The array of texel masks to be assigned to (must be width * height in size).
     */
    SGE_LIGHTMAPPER_API void generate_lightmap_texels(
        const LightmapObject* objects,
        std::size_t num_objects,
        int32 width,
        int32 height,
        LightmapTexel* out_texels,
        byte* out_texel_mask);

    /**
     * \brief Constructs a lightmap scene. You must call 'free_lightmap_scene' once you are done with this scene.
     * \param occluders The occluders within the scene.
     * WARNING: This value must outlive the scene.
     * \param num_occluders The number of occluders in the scene.
     * \return A new lightmap scene.
     */
    SGE_LIGHTMAPPER_API LightmapScene* new_lightmap_scene(
        const LightmapOccluder* occluders,
        std::size_t num_occluders);

    /**
     * \brief Frees resources associated with the given lightmap scene.
     * \param sene The scene to free.
     */
    SGE_LIGHTMAPPER_API void free_lightmap_scene(
        LightmapScene* sene);

	/**
	 * \brief Computes a fudgy ambient term. Useful for scenes where direct lighting is not the sole exterior contribution of light.
	 * \param ambient The amount of ambient light.
	 * \param width The width (in pixels) of the lightmap.
	 * \param height The height (in pixels) of the lightmap.
	 * \param texel_mask The array of texel masks in the lightmap.
	 * \param out_irradiance The pixels to assign the ambient radiance to.
	 */
	SGE_LIGHTMAPPER_API void compute_ambient_radiance(
		color::RGBF32 ambient,
		int32 width,
		int32 height,
		const byte* texel_mask,
		color::RGBF32* out_irradiance);

    /**
     * \brief Computes irradiance due to direct lighting from the given light source.
     * \param light The light source to compute direct irradiance with.
     * \param scene The scene to calculate occlusion against.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_irradiance The pixels to assign the direct radiance to (must be width * height in size).
     */
    SGE_LIGHTMAPPER_API void compute_direct_irradiance(
        const LightmapScene* scene,
        LightmapLight light,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* out_irradiance);

    /**
     * \brief Computes indirect radiance due to the given occluders.
     * \param scene The scene to calculate occlusion and irradiance against.
     * \param num_sample_sets The number of samples sets (8 samples) to gather.
     * \param num_accumulations The total number of accumulation passes that will be performed.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_x_basis_radiance Output for RGB radiance parallel to the x basis vector.
     * \param out_y_basis_radiance Output for RGB radiance parallel to the y basis vector.
     * \param out_z_basis_radiance Output for RGB radiance parallel to the z basis vector.
     * \param out_normal_irradiance Output for RGB irradiance parallel to the normal vector.
     */
    SGE_LIGHTMAPPER_API void compute_indirect_radiance(
        const LightmapScene* scene,
        int32 num_sample_sets,
		int32 num_accumulations,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* SGE_RESTRICT out_x_basis_radiance,
		color::RGBF32* SGE_RESTRICT out_y_basis_radiance,
		color::RGBF32* SGE_RESTRICT out_z_basis_radiance,
		color::RGBF32* SGE_RESTRICT out_normal_irradiance);

    /**
     * \brief Post-processes the given lightmap to reduce noise.
     * \param width The width of the lightmap.
     * \param height The height of the lightmap.
     * \param num_steps The number of post-processing iterations to perform.
     * \param lightmap Lightmap to post-process.
     */
    SGE_LIGHTMAPPER_API void lightmap_postprocess(
        int32 width,
        int32 height,
        int32 num_steps,
        color::RGBF32* lightmap);
}
