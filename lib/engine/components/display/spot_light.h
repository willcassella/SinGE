#pragma once

#include <stdint.h>

#include "lib/engine/component.h"
#include "lib/resource/misc/color.h"

namespace sge
{
    struct SGE_ENGINE_API CSpotlight
    {
        SGE_REFLECTED_TYPE;

        enum class Shape
        {
            /**
            * \brief This spotlight is in the shape of a cone.
            */
            CONE,

            /**
            * \brief This spotlight is in the shape of a frustum.
            */
            FRUSTUM
        };

        struct SharedData;

        explicit CSpotlight(NodeId node, SharedData& shared_data);

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

        uint32_t update_revision() const;

        Shape shape() const;

        void shape(Shape shape);

        void set_cone(Angle angle);

        void set_frustum(Angle horiz_angle, Angle vert_angle);

        Angle cone_angle() const;

        void cone_angle(Angle value);

        Angle frustum_horiz_angle() const;

        void frustum_horiz_angle(Angle value);

        Angle frustum_vert_angle() const;

        void frustum_vert_angle(Angle value);

        float near_clipping_plane() const;

        void near_clipping_plane(float value);

        float far_clipping_plane() const;

        void far_clipping_plane(float value);

        color::RGBF32 intensity() const;

        void intensity(color::RGBF32 value);

        bool casts_shadows() const;

        void casts_shadows(bool value);

        uint32_t shadow_map_width() const;

        void shadow_map_width(uint32_t value);

        uint32_t shadow_map_height() const;

        void shadow_map_height(uint32_t value);

        bool is_lightmask_volume() const;

        void is_lightmask_volume(bool value);

        uint32_t lightmask_group() const;

        void lightmask_group(uint32_t value);

    private:
        void set_modified(const char* property_name);

        uint32_t _update_revision = 0;
        Shape _shape = Shape::CONE;
        Angle _cone_angle = degrees(30.f);
        Angle _frustum_horiz_angle = degrees(30.f);
        Angle _frustum_vert_angle = degrees(30.f);
        float _near_clipping_plane = 0.1f;
        float _far_clipping_plane = 1.0f;
        color::RGBF32 _intensity = color::RGBF32::white();
        bool _casts_shadows = false;
        uint32_t _shadow_width = 512;
        uint32_t _shadow_height = 512;
        bool _lightmask_volume = false;
        uint32_t _lightmask_group = 0;
        NodeId _node;
        SharedData* _shared_data = nullptr;
    };
}

SGE_REFLECTED_ENUM(SGE_ENGINE_API, sge::CSpotlight::Shape);
