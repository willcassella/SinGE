// CLightMaskReceiver.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightMaskReceiver.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightMaskReceiver);

namespace sge
{
	struct CLightMaskReceiver::SharedData : CSharedData<CLightMaskReceiver>
	{
	};

	CLightMaskReceiver::CLightMaskReceiver(NodeId node, SharedData& /*shared_data*/)
		: _node(node)
	{
	}

    void CLightMaskReceiver::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CLightMaskReceiver, SharedData>>());
    }

	void CLightMaskReceiver::to_archive(ArchiveWriter& /*writer*/) const
	{
	}

	void CLightMaskReceiver::from_archive(ArchiveReader& /*reader*/)
	{
	}

	NodeId CLightMaskReceiver::node() const
	{
		return _node;
	}
}
