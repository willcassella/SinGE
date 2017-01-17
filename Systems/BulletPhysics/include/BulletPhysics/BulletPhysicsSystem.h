// BulletPhysicsSystem.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Engine/Scene.h>
#include "build.h"

namespace sge
{
    namespace bullet_physics
    {
        struct Config;

        struct SGE_BULLET_PHYSICS_API BulletPhysicsSystem
        {
            struct Data;
            SGE_REFLECTED_TYPE;

            ////////////////////////
            ///   Constructors   ///
        public:

            BulletPhysicsSystem(const Config& config);

            ///////////////////
            ///   Methods   ///
        public:

            void register_with_scene(Scene& scene);

            void unregister_with_scene(Scene& scene);

        private:

            void phys_tick(SystemFrameMut& frame, float current_time, float dt);

            //////////////////
            ///   Fields   ///
        private:

            Scene::SystemFnToken _update_phys_token;
            std::unique_ptr<Data> _data;
        };
    }
}
