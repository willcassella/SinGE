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

                // Create an infinite plane
                _ground_shape = std::make_unique<btStaticPlaneShape>(btVector3{ 0, 1, 0 }, 1.f);
                _ground_motion_state = std::make_unique<btDefaultMotionState>(btTransform(btQuaternion{ 0, 0, 0, 1 }, btVector3{ 0, -1, 0 }));

                btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI{
                    0.f,
                    _ground_motion_state.get(),
                    _ground_shape.get(),
                    btVector3(0, 0, 0) };
                _ground_body = std::make_unique<btRigidBody>(groundRigidBodyCI);
                _dynamics_world->addRigidBody(_ground_body.get());

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

            std::unique_ptr<btStaticPlaneShape> _ground_shape;
            std::unique_ptr<btDefaultMotionState> _ground_motion_state;
            std::unique_ptr<btRigidBody> _ground_body;

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
