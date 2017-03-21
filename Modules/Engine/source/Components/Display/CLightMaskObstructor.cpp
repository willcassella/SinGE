// CLightMaskObstructor.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../../include/Engine/Components/Display/CLightMaskObstructor.h"
#include "../../../include/Engine/Util/BasicComponentContainer.h"
#include "../../../include/Engine/Util/CSharedData.h"
#include "../../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::CLightMaskObstructor);

namespace sge
{
	struct CLightMaskObstructor::SharedData : CSharedData<CLightMaskObstructor>
	{
	};

	CLightMaskObstructor::CLightMaskObstructor(NodeId node, SharedData& /*shared_data*/)
		: _node(node)
	{
	}

	void CLightMaskObstructor::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CLightMaskObstructor, SharedData>>());
	}

	void CLightMaskObstructor::to_archive(ArchiveWriter& /*writer*/) const
	{
	}

	void CLightMaskObstructor::from_archive(ArchiveReader& /*reader*/)
	{
	}

	NodeId CLightMaskObstructor::node() const
	{
		return _node;
	}
}
