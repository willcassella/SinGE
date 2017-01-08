// Component.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/Components/CTransform3D.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"

SGE_REFLECT_TYPE(sge::ComponentId);
SGE_REFLECT_TYPE(sge::FNewComponent);
SGE_REFLECT_TYPE(sge::FDestroyedComponent);

namespace sge
{
	void ComponentInterface::from_property_archive(ArchiveReader& reader)
	{
		const auto& type = this->get_type();

		reader.enumerate_object_members([this, &type, &reader](const char* propName)
		{
			const auto* prop = type.find_property(propName);
			if (prop)
			{
				prop->mutate(this, [&reader](AnyMut<> value)
				{
					const auto* fromArchive = sge::get_vtable<IFromArchive>(value.type());
					if (fromArchive)
					{
						fromArchive->from_archive(value.object(), reader);
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
