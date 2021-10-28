set(SOURCE_FILES
    source/BulletPhysicsSystem.cpp
    source/BulletPhysicsSystemData.cpp
    source/CharacterController.cpp
    source/Colliders.cpp
    source/Config.cpp
    source/DebugDrawer.cpp
    source/LevelPortal.cpp
    source/LightmaskVolumeCollider.cpp
    source/PhysicsEntity.cpp
    source/RigidBody.cpp
)
add_library(BulletPhysics SHARED ${SOURCE_FILES})
target_compile_definitions(BulletPhysics PRIVATE SGE_BULLET_PHYSICS_BUILD)
target_include_directories(BulletPhysics PUBLIC include)
target_include_directories(BulletPhysics PRIVATE private)
target_link_libraries(BulletPhysics PUBLIC Core Resource Engine)

pkg_check_modules(Bullet bullet REQUIRED)
target_include_directories(BulletPhysics SYSTEM PRIVATE ${Bullet_STATIC_INCLUDE_DIRS})
target_link_libraries(BulletPhysics PRIVATE ${Bullet_STATIC_LIBRARIES})
