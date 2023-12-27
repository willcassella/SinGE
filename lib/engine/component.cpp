#include "lib/engine/components/display/camera.h"
#include "lib/engine/components/display/point_light.h"
#include "lib/engine/components/display/spot_light.h"
#include "lib/engine/components/display/static_mesh.h"
#include "lib/engine/components/gameplay/animation.h"
#include "lib/engine/components/gameplay/character_controller.h"
#include "lib/engine/components/gameplay/input.h"
#include "lib/engine/components/gameplay/level_portal.h"
#include "lib/engine/components/physics/box_collider.h"
#include "lib/engine/components/physics/capsule_collider.h"
#include "lib/engine/components/physics/rigid_body.h"
#include "lib/engine/components/physics/sensor.h"
#include "lib/engine/components/physics/sphere_collider.h"
#include "lib/engine/components/physics/static_mesh_collider.h"
#include "lib/engine/scene.h"

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
        CLevelPortal::register_type(scene);
        CAnimation::register_type(scene);
    }
}
