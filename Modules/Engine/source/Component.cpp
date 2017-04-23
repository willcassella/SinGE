// Component.cpp

#include "../include/Engine/Scene.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"
#include "../include/Engine/Components/Display/CSpotlight.h"
#include "../include/Engine/Components/Display/CPointLight.h"
#include "../include/Engine/Components/Gameplay/CInput.h"
#include "../include/Engine/Components/Physics/CSphereCollider.h"
#include "../include/Engine/Components/Physics/CBoxCollider.h"
#include "../include/Engine/Components/Physics/CCapsuleCollider.h"
#include "../include/Engine/Components/Physics/CStaticMeshCollider.h"
#include "../include/Engine/Components/Physics/CSensor.h"
#include "../include/Engine/Components/Physics/CRigidBody.h"
#include "../include/Engine/Components/Gameplay/CCharacterController.h"

namespace sge
{
    void register_builtin_components(Scene& scene)
	{
		CStaticMesh::register_type(scene);
		CPerspectiveCamera::register_type(scene);
		CSpotlight::register_type(scene);
        CInput::register_type(scene);
		CSphereCollider::register_type(scene);
        CBoxCollider::register_type(scene);
		CStaticMeshCollider::register_type(scene);
        CCapsuleCollider::register_type(scene);
        CSensor::register_type(scene);
        CRigidBody::register_type(scene);
		CPointLight::register_type(scene);
        CCharacterController::register_type(scene);
	}
}
