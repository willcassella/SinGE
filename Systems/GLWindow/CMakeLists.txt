set(SOURCE_FILES
    source/GLEventWindow.cpp
    source/InputBindings.cpp
)
add_library(GLWindow STATIC ${SOURCE_FILES})
target_include_directories(GLWindow PUBLIC include)
target_link_libraries(GLWindow PUBLIC Core Resource Engine)

pkg_check_modules(GLFW glfw3 REQUIRED)
target_include_directories(GLWindow SYSTEM PUBLIC ${GLFW_STATIC_INCLUDE_DIRS})
target_link_libraries(GLWindow PUBLIC ${GLFW_STATIC_LIBRARIES})
