// CubeCollider.cpp

#include "../../../include/Engine/Components/Physics/CubeCollider.h"
#include "../../../include/Engine/Component.h"

REFLECT_TYPE(singe::CubeCollider)
.extends<singe::Collider>()
.auto_impl<singe::Component>();
