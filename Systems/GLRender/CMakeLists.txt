# GLRender project CMake file
cmake_minimum_required(VERSION 2.8)
project(GLRender CXX)

# Private
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/source/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_definitions(-DSGE_GLRENDER_BUILD)

# Dependencies
pkg_check_modules(Glew glew REQUIRED)
include_directories(${Core_INCLUDE_DIRS} ${Resource_INCLUDE_DIRS} ${Engine_INCLUDE_DIRS} ${Glew_STATIC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Core Resource Engine ${Glew_STATIC_LIBRARIES})

# Public
set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)
