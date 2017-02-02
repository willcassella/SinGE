# GLWindow project cmake file
cmake_minimum_required(VERSION 2.8)
project(GLWindow CXX)

# Private
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/source/*.cpp")
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# Dependencies
pkg_check_modules(GLFW glfw3 REQUIRED)
include_directories(${GLFW_STATIC_INCLUDE_DIRS})
include_directories(${Core_INCLUDE_DIRS} ${Resource_INCLUDE_DIRS} ${Engine_INCLUDE_DIRS})

# Public
set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)
