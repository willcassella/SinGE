#include <stdint.h>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/components/display/spot_light.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"

SGE_REFLECT_TYPE(sge::CSpotlight)
.property("node", &CSpotlight::node, nullptr)
.property("update_revision", &CSpotlight::update_revision, nullptr)
.property("shape", &CSpotlight::shape, &CSpotlight::shape)
.property("cone_angle", &CSpotlight::cone_angle, &CSpotlight::cone_angle)
.property("frustum_horiz_angle", &CSpotlight::frustum_horiz_angle, &CSpotlight::frustum_horiz_angle)
.property("frustum_vert_angle", &CSpotlight::frustum_vert_angle, &CSpotlight::frustum_vert_angle)
.property("near_clipping_plane", &CSpotlight::near_clipping_plane, &CSpotlight::near_clipping_plane)
.property("far_clipping_plane", &CSpotlight::far_clipping_plane, &CSpotlight::far_clipping_plane)
.property("intensity", &CSpotlight::intensity, &CSpotlight::intensity)
.property("casts_shadows", &CSpotlight::casts_shadows, &CSpotlight::casts_shadows)
.property("shadow_map_width", &CSpotlight::shadow_map_width, &CSpotlight::shadow_map_width)
.property("shadow_map_height", &CSpotlight::shadow_map_height, &CSpotlight::shadow_map_height)
.property("lightmask_volume", &CSpotlight::is_lightmask_volume, &CSpotlight::is_lightmask_volume)
.property("lightmask_group", &CSpotlight::lightmask_group, &CSpotlight::lightmask_group);

SGE_REFLECT_ENUM(sge::CSpotlight::Shape)
.value("CONE", CSpotlight::Shape::CONE)
.value("FRUSTUM", CSpotlight::Shape::FRUSTUM);

namespace sge
{
    struct CSpotlight::SharedData : CSharedData<CSpotlight>
    {
    };

    CSpotlight::CSpotlight(NodeId node, SharedData& shared_data)
        : _node(node),
        _shared_data(&shared_data)
    {
    }

    void CSpotlight::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CSpotlight, SharedData>>());
    }

    void CSpotlight::to_archive(ArchiveWriter& writer) const
    {
        if (_shape == Shape::CONE)
        {
            writer.object_member("cone", _cone_angle);
        }
        else if (_shape == Shape::FRUSTUM)
        {
            writer.push_object_member("frustum");
            writer.object_member("horiz", _frustum_horiz_angle);
            writer.object_member("vert", _frustum_vert_angle);
            writer.pop();
        }

        writer.object_member("near", _near_clipping_plane);
        writer.object_member("far", _far_clipping_plane);
        writer.object_member("int", _intensity);
        writer.object_member("shdw", _casts_shadows);
        writer.object_member("shdw_w", _shadow_width);
        writer.object_member("shdw_h", _shadow_height);
        writer.object_member("lmv", _lightmask_volume);
        writer.object_member("lmg", _lightmask_group);
    }

    void CSpotlight::from_archive(ArchiveReader& reader)
    {
        if (reader.pull_object_member("cone"))
        {
            _shape = Shape::CONE;
            _cone_angle.from_archive(reader);
            reader.pop();
        }
        else if (reader.pull_object_member("frustum"))
        {
            _shape = Shape::FRUSTUM;
            reader.object_member("horiz", _frustum_horiz_angle);
            reader.object_member("vert", _frustum_vert_angle);
            reader.pop();
        }

        reader.object_member("near", _near_clipping_plane);
        reader.object_member("far", _far_clipping_plane);
        reader.object_member("int", _intensity);
        reader.object_member("shdw", _casts_shadows);
        reader.object_member("shdw_w", _shadow_width);
        reader.object_member("shdw_h", _shadow_height);
        reader.object_member("lmv", _lightmask_volume);
        reader.object_member("lmg", _lightmask_group);
    }

    NodeId CSpotlight::node() const
    {
        return _node;
    }

    uint32_t CSpotlight::update_revision() const
    {
        return _update_revision;
    }

    CSpotlight::Shape CSpotlight::shape() const
    {
        return _shape;
    }

    void CSpotlight::shape(Shape shape)
    {
        if (_shape != shape)
        {
            _shape = shape;
            set_modified("shape");
        }
    }

    void CSpotlight::set_cone(Angle angle)
    {
        shape(Shape::CONE);
        cone_angle(angle);
    }

    void CSpotlight::set_frustum(Angle horiz_angle, Angle vert_angle)
    {
        shape(Shape::FRUSTUM);
        frustum_horiz_angle(horiz_angle);
        frustum_vert_angle(vert_angle);
    }

    Angle CSpotlight::cone_angle() const
    {
        return _cone_angle;
    }

    void CSpotlight::cone_angle(Angle value)
    {
        _cone_angle = value;
        set_modified("cone_angle");
    }

    Angle CSpotlight::frustum_horiz_angle() const
    {
        return _frustum_horiz_angle;
    }

    void CSpotlight::frustum_horiz_angle(Angle value)
    {
        _frustum_horiz_angle = value;
        set_modified("frustum_horiz_angle");
    }

    Angle CSpotlight::frustum_vert_angle() const
    {
        return _frustum_vert_angle;
    }

    void CSpotlight::frustum_vert_angle(Angle value)
    {
        _frustum_vert_angle = value;
        set_modified("frustum_vert_angle");
    }

    float CSpotlight::near_clipping_plane() const
    {
        return _near_clipping_plane;
    }

    void CSpotlight::near_clipping_plane(float value)
    {
        if (value > _far_clipping_plane)
        {
            return;
        }

        _near_clipping_plane = value;
        set_modified("near_clipping_plane");
    }

    float CSpotlight::far_clipping_plane() const
    {
        return _far_clipping_plane;
    }

    void CSpotlight::far_clipping_plane(float value)
    {
        if (value < _near_clipping_plane)
        {
            return;
        }

        _far_clipping_plane = value;
        set_modified("far_clipping_plane");
    }

    color::RGBF32 CSpotlight::intensity() const
    {
        return _intensity;
    }

    void CSpotlight::intensity(color::RGBF32 value)
    {
        _intensity = value;
        set_modified("intensity");
    }

    bool CSpotlight::casts_shadows() const
    {
        return _casts_shadows;
    }

    void CSpotlight::casts_shadows(bool value)
    {
        if (_casts_shadows != value)
        {
            _casts_shadows = value;
            set_modified("casts_shadows");
        }
    }

    uint32_t CSpotlight::shadow_map_width() const
    {
        return _shadow_width;
    }

    void CSpotlight::shadow_map_width(uint32_t value)
    {
        if (_shadow_width != value)
        {
            _shadow_width = value;
            set_modified("shadow_map_width");
        }
    }

    uint32_t CSpotlight::shadow_map_height() const
    {
        return _shadow_height;
    }

    void CSpotlight::shadow_map_height(uint32_t value)
    {
        if (_shadow_height != value)
        {
            _shadow_height = value;
            set_modified("shadow_map_height");
        }
    }

    bool CSpotlight::is_lightmask_volume() const
    {
        return _lightmask_volume;
    }

    void CSpotlight::is_lightmask_volume(bool value)
    {
        if (value != _lightmask_volume)
        {
            _lightmask_volume = value;
            set_modified("lightmask_volume");
        }
    }

    uint32_t CSpotlight::lightmask_group() const
    {
        return _lightmask_group;
    }

    void CSpotlight::lightmask_group(uint32_t value)
    {
        if (value != _lightmask_group)
        {
            _lightmask_group = value;
            set_modified("lightmask_group");
        }
    }

    void CSpotlight::set_modified(const char* property_name)
    {
        _update_revision += 1;
        _shared_data->set_modified(_node, this, property_name);
    }
}
