set(SOURCE_FILES
    source/main.cpp
)
add_executable(GLClient ${SOURCE_FILES})
target_link_libraries(GLClient PRIVATE Core Resource Engine GLRender BulletPhysics GLWindow)

pkg_check_modules(GLFW glfw3 REQUIRED)
target_include_directories(GLClient SYSTEM PRIVATE ${GLFW_STATIC_INCLUDE_DIRS})
target_link_libraries(GLClient PRIVATE ${GLFW_STATIC_LIBRARIES})
