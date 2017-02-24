// Lightmapper.h
#pragma once

#include <Core/Math/Mat4.h>
#include <Resource/Resources/Image.h>
#include <Resource/Resources/StaticMesh.h>
#include "build.h"

namespace sge
{
    struct LightmapTexel
    {
        //////////////////
        ///   Fields   ///
    public:

        Mat4 TBN;
        Vec4 world_pos;
    };

    struct LightmapObject
    {
        //////////////////
        ///   Fields   ///
    public:

        const StaticMesh* mesh;
        Mat4 transform;
        Vec2 lightmap_pos;
        float lightmap_exts;
    };

    struct LightmapOccluder
    {
        //////////////////
        ///   Fields   ///
    public:

        const StaticMesh* mesh;
        const Image* albedo;
        const color::RGBF32* lightmap;
        Mat4 transform;
    };

    struct LightmapLight
    {
        //////////////////
        ///   Fields   ///
    public:

        Vec3 direction;
        color::RGBF32 color;
    };

    /**
     * \brief Packs the lightmap UV layouts for the given meshes (and their transforms) into the given lightmap.
     * \param objects An array of objects to be put into the lightmap. The 'mesh' and 'transform' fields must be initialized,
     * the 'lightmap_pos' and 'lightmap_exts' fields will be initialized by this function.
     * \param num_objects The number of objects in the array.
     */
    void SGE_LIGHTMAPPER_API generate_lightmap_pack(
        LightmapObject* objects,
        std::size_t num_objects);

    /**
     * \brief Generates lightmap texel information for the given lightmap objects.
     * \param objects An array of objects in the lightmap. All fields must be initialized.
     * \param num_objects The number of objects in the lightmap.
     * \param width The width (in pixels) of the lightmap.
     * \param height The hight (in pixels) of the lightmap.
     * \param out_texels The array of texels to be assigned to (must be width * height in size).
     * \param out_texel_mask The array of texel masks to be assigned to (must be width * height in size).
     */
    void SGE_LIGHTMAPPER_API generate_lightmap_texels(
        const LightmapObject* objects,
        std::size_t num_objects,
        int32 width,
        int32 height,
        LightmapTexel* out_texels,
        byte* out_texel_mask);

    /**
     * \brief Computes direct irradiance due to the given light source.
     * \param light The light source to compute direct irradiance with.
     * \param occluders Objects that may occlude the light source. This may include objects that are not a part of the light map.
     * \param num_occluders The number of occluding objects.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_irradiance The pixels to assign the direct irradiance to (must be width * height in size).
     */
    void SGE_LIGHTMAPPER_API compute_direct_irradiance(
        LightmapLight light,
        const LightmapOccluder* occluders,
        std::size_t num_occluders,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* out_irradiance);

    /**
     * \brief Computes indirect irradiance due to the given occluders.
     * \param occluders Objects to compute indirect irradiance against. This may include objects that are not a part of the light map.
     * \param num_occluders The number of occluding objects.
     * \param width The width (in pixels) of the light map.
     * \param height The height (in pixels) of the light map.
     * \param texels The array of texels in the light map (must be width * height in size).
     * \param texel_mask The array of texel masks in the light map (must be width * height in size).
     * \param out_irradiance The pixels to assign the direct irradiance to (must be width * height in size).
     */
    void SGE_LIGHTMAPPER_API compute_indirect_irradiance(
        const LightmapOccluder* occluders,
        std::size_t num_occluders,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* out_irradiance);
}
