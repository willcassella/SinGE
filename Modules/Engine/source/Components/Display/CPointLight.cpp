// CPointLight.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CPointLight.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CPointLight)
.property("node", &CPointLight::node, nullptr)
.property("update_revision", &CPointLight::update_revision, nullptr)
.property("radius", &CPointLight::radius, &CPointLight::radius)
.property("intensity", &CPointLight::intensity, &CPointLight::intensity);

namespace sge
{
    struct CPointLight::SharedData : CSharedData<CPointLight>
    {
    };

    CPointLight::CPointLight(const NodeId node_id, SharedData& shared_data)
        : _node(node_id),
        _shared_data(&shared_data)
    {
    }

    void CPointLight::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CPointLight, SharedData>>());
    }

    void CPointLight::to_archive(ArchiveWriter& writer) const
    {
        writer.as_object();
        writer.object_member("r", _radius);
        writer.object_member("i", _intensity);
    }

    void CPointLight::from_archive(ArchiveReader& reader)
    {
        reader.object_member("r", _radius);
        reader.object_member("i", _intensity);
    }

    NodeId CPointLight::node() const
    {
        return _node;
    }

    uint32 CPointLight::update_revision() const
    {
        return _update_revision;
    }

    float CPointLight::radius() const
    {
        return _radius;
    }

    void CPointLight::radius(float value)
    {
        _radius = value;
        set_modified("radius");
    }

    color::RGBF32 CPointLight::intensity() const
    {
        return _intensity;
    }

    void CPointLight::intensity(color::RGBF32 value)
    {
        _intensity = value;
        set_modified("intensity");
    }

    void CPointLight::set_modified(const char* prop_name)
    {
        _update_revision += 1;
        _shared_data->set_modified(_node, this, prop_name);
    }
}
