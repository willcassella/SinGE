#pragma once

#include <map>
#include <stdint.h>

#include "lib/engine/component.h"
#include "lib/resource/misc/color.h"

namespace sge
{
    struct SGE_ENGINE_API SceneLightmap
    {
        SGE_REFLECTED_TYPE;

        struct LightmapElement
        {
            int32_t width;
            int32_t height;
            std::vector<color::RGBF32> basis_x_radiance;
            std::vector<color::RGBF32> basis_y_radiance;
            std::vector<color::RGBF32> basis_z_radiance;
            std::vector<uint8_t> direct_mask;
        };

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        Vec3 light_direction;
        color::RGBF32 light_intensity;
        std::map<NodeId, LightmapElement> lightmap_elements;
    };
}
