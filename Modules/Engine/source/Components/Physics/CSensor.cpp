// CSensor.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Physics/CSensor.h"
#include "../../../include/Engine/Scene.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"

SGE_REFLECT_TYPE(sge::CSensor);

namespace sge
{
    struct CSensor::SharedData : CSharedData<CSensor>
    {
    };

    CSensor::CSensor(NodeId node, SharedData& /*shared_data*/)
        : _node(node)
    {
    }

    void CSensor::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CSensor, SharedData>>());
    }

    void CSensor::to_archive(ArchiveWriter& /*writer*/) const
    {
    }

    void CSensor::from_archive(ArchiveReader& /*reader*/)
    {
    }

    NodeId CSensor::node() const
    {
        return _node;
    }
}
