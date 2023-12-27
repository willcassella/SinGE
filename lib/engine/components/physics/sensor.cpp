#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/components/physics/sensor.h"
#include "lib/engine/scene.h"
#include "lib/engine/util/basic_component_container.h"
#include "lib/engine/util/shared_data.h"

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
