set(SOURCE_FILES
    source/Component.cpp
    source/Components/Display/CCamera.cpp
    source/Components/Display/CPointLight.cpp
    source/Components/Display/CSpotLight.cpp
    source/Components/Display/CStaticMesh.cpp
    source/Components/Gameplay/CAnimation.cpp
    source/Components/Gameplay/CCharacterController.cpp
    source/Components/Gameplay/CInput.cpp
    source/Components/Gameplay/CLevelPortal.cpp
    source/Components/Physics/CBoxCollider.cpp
    source/Components/Physics/CCapsuleCollider.cpp
    source/Components/Physics/CRigidBody.cpp
    source/Components/Physics/CSensor.cpp
    source/Components/Physics/CSphereCollider.cpp
    source/Components/Physics/CStaticMeshCollider.cpp
    source/Engine/UpdatePipeline.cpp
    source/EventChannel.cpp
    source/Lightmap.cpp
    source/Node.cpp
    source/Scene.cpp
    source/SystemFrame.cpp
    source/Systems/AnimationSystem.cpp
    source/Systems/ChangeLevelSystem.cpp
)
add_library(Engine SHARED ${SOURCE_FILES})
target_compile_definitions(Engine PRIVATE SGE_ENGINE_BUILD)
target_include_directories(Engine PUBLIC include)
target_link_libraries(Engine PUBLIC Core Resource)
