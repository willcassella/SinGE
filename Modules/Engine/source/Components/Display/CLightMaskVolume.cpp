// CLightMaskVolume.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightMaskVolume.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightMaskVolume);

namespace sge
{
	struct CLightMaskVolume::SharedData : CSharedData<CLightMaskVolume>
	{
	};

	CLightMaskVolume::CLightMaskVolume(NodeId node, SharedData& /*shared_data*/)
		: _node(node)
	{
	}

    void CLightMaskVolume::register_type(Scene& scene)
    {
        scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CLightMaskVolume, SharedData>>());
    }

	void CLightMaskVolume::to_archive(ArchiveWriter& /*writer*/) const
	{
	}

	void CLightMaskVolume::from_archive(ArchiveReader& /*reader*/)
	{
	}

	NodeId CLightMaskVolume::node() const
	{
		return _node;
	}
}
