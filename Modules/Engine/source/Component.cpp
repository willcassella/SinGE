// Component.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/Components/CTransform3D.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"

SGE_REFLECT_TYPE(sge::ComponentId);
SGE_REFLECT_TYPE(sge::ComponentInstanceMut);
SGE_REFLECT_TYPE(sge::ComponentInstance);
SGE_REFLECT_TYPE(sge::ComponentContext);

namespace sge
{
	void register_builtin_components(Scene& scene)
	{
		scene.register_component_type<CTransform3D>();
		scene.register_component_type<CPerspectiveCamera>();
		scene.register_component_type<COrthographicCamera>();
		scene.register_component_type<CStaticMesh>();
	}
}
