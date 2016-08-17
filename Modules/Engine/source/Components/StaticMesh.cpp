// StaticMesh.cpp

#include "../../include/Engine/Components/StaticMesh.h"
#include "../../include/Engine/Component.h"

REFLECT_TYPE(singe::StaticMesh)
.auto_impl<singe::Component>();

namespace singe
{
	StaticMesh::StaticMesh()
	{
		visible = true;
	}
}