// SystemData.h
#pragma once

#include <unordered_map>
#include <Engine/Component.h>
#include "../include/BulletPhysics/BulletPhysicsSystem.h"
#include "Collider.h"
#include "PhysicsWorld.h"

namespace sge
{
    namespace bullet_physics
    {
        struct BulletPhysicsSystem::Data
        {
            //////////////////////
            ///   Scene data   ///
        public:

            PhysicsWorld phys_world;

            std::unordered_map<EntityId, std::unique_ptr<Collider>> colliders;
        };
    }
}
