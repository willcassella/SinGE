// BulletPhysicsSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "../private/BulletPhysicsSystemData.h"

SGE_REFLECT_TYPE(sge::bullet_physics::BulletPhysicsSystem);

namespace sge
{
    namespace bullet_physics
    {
        BulletPhysicsSystem::BulletPhysicsSystem(const Config& config)
            : _update_phys_token(Scene::NULL_SYSTEM_TOKEN)
        {
            _data = std::make_unique<Data>();
        }

        void BulletPhysicsSystem::register_with_scene(Scene& scene)
        {
            _update_phys_token = scene.register_system_mut_fn(this, &BulletPhysicsSystem::phys_tick);
        }

        void BulletPhysicsSystem::unregister_with_scene(Scene& scene)
        {
            scene.unregister_system_fn(_update_phys_token);
            _update_phys_token = Scene::NULL_SYSTEM_TOKEN;
        }

        void BulletPhysicsSystem::phys_tick(SystemFrameMut& frame, float current_time, float dt)
        {
            _data->phys_world.dynamics_world().stepSimulation(dt, 1);
        }
    }
}
