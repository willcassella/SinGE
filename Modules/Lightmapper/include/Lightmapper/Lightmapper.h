// Lightmapper.h
#pragma once

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
         * \brief Base color of this object, used for irradiance transmission.
         */
        color::RGBF32 base_color;

        /**
         * \brief Width (in pixels) of the irradiance map for this object.
         */
        int32 irradiance_width = 0;

        /**
         * \brief Height (in pixels) of the irradiance map for this object.
         */
        int32 irradiance_height = 0;

        /**
         * \brief The irradiance of this object.
         */
        const color::RGBAF32* irradiance = nullptr;
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
     * \brief Computes direct irradiance due to the given light source.
     * \param light The light source to compute direct irradiance with.
     * \param scene The scene to calculate occlusion against.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_irradiance The pixels to assign the direct irradiance to (must be width * height in size).
     */
    SGE_LIGHTMAPPER_API void compute_direct_irradiance(
        const LightmapScene* scene,
        LightmapLight light,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBAF32* out_irradiance);

    /**
     * \brief Computes indirect irradiance due to the given occluders.
     * \param scene The scene to calculate occlusion and irradiance against.
     * \param num_sample_sets The number of samples sets (8 samples) to gather.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_irradiance The pixels to assign the direct irradiance to (must be width * height in size).
     */
    SGE_LIGHTMAPPER_API void compute_indirect_irradiance(
        const LightmapScene* scene,
        int32 num_sample_sets,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBAF32* out_irradiance);

    /**
     * \brief Post-processes the lightmap to reduce noise.
     * \param width The width of the lightmap.
     * \param height The height of the lightmap.
     * \param num_steps The number of post-processing iterations to perform.
     * \param irradiance Irradiance map to post-process.
     */
    SGE_LIGHTMAPPER_API void postprocess_irradiance(
        int32 width,
        int32 height,
        int32 num_steps,
        color::RGBAF32* irradiance);
}
