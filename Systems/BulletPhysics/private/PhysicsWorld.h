// PhysicsWorld.h
#pragma once

#include <memory>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

namespace sge
{
    namespace bullet_physics
    {
        struct PhysicsWorld
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            PhysicsWorld()
            {
                // Initialize physics configuration
                _broad_phase_interface = std::make_unique<btDbvtBroadphase>();
                _collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
                _dispatcher = std::make_unique<btCollisionDispatcher>(_collision_configuration.get());
                btGImpactCollisionAlgorithm::registerAlgorithm(_dispatcher.get());

                // Initialize the physics world
                _dynamics_world = std::make_unique<btDiscreteDynamicsWorld>(
                    _dispatcher.get(), 
                    _broad_phase_interface.get(), 
                    _constraint_solver.get(), 
                    _collision_configuration.get());
                _dynamics_world->setGravity(btVector3{ 0, -10, 0 });

                // I have to do this for some reason
                _ghost_callback = std::make_unique<btGhostPairCallback>();
                _dynamics_world->getPairCache()->setInternalGhostPairCallback(_ghost_callback.get());
            }

            ///////////////////
            ///   Methods   ///
        public:

            /** Returns the dynamics world. */
            btDynamicsWorld& dynamics_world()
            {
                return *_dynamics_world;
            }

            /** Returns the dynamics world. */
            const btDynamicsWorld& dynamics_world() const
            {
                return *_dynamics_world;
            }

            //////////////////
            ///   Fields   ///
        private:

            // Physics configuration
            std::unique_ptr<btBroadphaseInterface> _broad_phase_interface;
            std::unique_ptr<btCollisionConfiguration> _collision_configuration;
            std::unique_ptr<btCollisionDispatcher> _dispatcher;
            std::unique_ptr<btConstraintSolver> _constraint_solver;
            std::unique_ptr<btGhostPairCallback> _ghost_callback;

            // Physics world
            std::unique_ptr<btDynamicsWorld> _dynamics_world;
        };
    }
}
