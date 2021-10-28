set(SOURCE_FILES
    source/Config.cpp
    source/GLMaterial.cpp
    source/GLRenderSystem.cpp
    source/GLRenderSystemState.cpp
    source/GLShader.cpp
    source/GLStaticMesh.cpp
    source/GLTexture2D.cpp
    source/RenderCommands.cpp
    source/RenderResource.cpp
    source/RenderScene.cpp
    source/Util.cpp
)
add_library(GLRender SHARED ${SOURCE_FILES})
target_compile_definitions(GLRender PRIVATE SGE_GLRENDER_BUILD)
target_include_directories(GLRender PUBLIC include)
target_include_directories(GLRender PRIVATE private)
target_link_libraries(GLRender PUBLIC Core Resource Engine)

pkg_check_modules(Glew glew REQUIRED)
target_include_directories(GLRender SYSTEM PRIVATE ${Glew_STATIC_INCLUDE_DIRS})
target_link_libraries(GLRender PRIVATE ${Glew_STATIC_LIBRARIES})
