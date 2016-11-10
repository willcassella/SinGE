// Component.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/Components/CTransform3D.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"

SGE_REFLECT_TYPE(sge::ComponentId);

namespace sge
{
	void ComponentInterface::from_archive(const ArchiveReader& reader)
	{
		const auto& type = this->get_type();

		reader.enumerate_object_members(
			[this, &type](const char* propName, const ArchiveReader& propReader)
		{
			const auto* prop = type.find_property(propName);
			if (prop)
			{
				prop->mutate(this, nullptr,
					[&propReader](AnyMut<> value)
				{
					const auto* fromArchive = sge::get_vtable<IFromArchive>(value.type());
					if (fromArchive)
					{
						fromArchive->from_archive(value.object(), propReader);
					}
				});
			}
		});
	}

	void register_builtin_components(Scene& scene)
	{
		CTransform3D::register_type(scene);
		CStaticMesh::register_type(scene);
		CPerspectiveCamera::register_type(scene);
	}
}
