# GLEditorServer runtime CMake file
cmake_minimum_required(VERSION 2.8)
project(GLEditorServer CXX)

# Private
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/source/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Dependencies
pkg_check_modules(GLFW glfw3 REQUIRED)
include_directories(${GLFW_STATIC_INCLUDE_DIRS})
include_directories(${Core_INCLUDE_DIRS} ${Resource_INCLUDE_DIRS} ${Engine_INCLUDE_DIRS} ${GLRender_INCLUDE_DIRS} ${EditorServerSystem_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Core Resource Engine GLRender EditorServerSystem ${GLFW_STATIC_LIBRARIES})
