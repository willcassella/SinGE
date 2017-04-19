# Lightmapper project CMake file
cmake_minimum_required(VERSION 2.8)
project(Lightmapper CXX)

# Private
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/source/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_definitions(-DSGE_LIGHTMAPPER_BUILD)

# Dependencies
include_directories(${Core_INCLUDE_DIRS} ${Resource_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Core Resource embree)

# Public
set(${PROJECT_NAME}_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)
