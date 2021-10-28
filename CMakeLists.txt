cmake_minimum_required(VERSION 3.16)
project(
    SinGE
    VERSION 1.0
    DESCRIPTION "A C++ Game Engine"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 14)

if (${UNIX})
    find_package(PkgConfig REQUIRED)
endif()

add_subdirectory(Modules/Core)
add_subdirectory(Modules/Resource)
add_subdirectory(Modules/Engine)
add_subdirectory(Systems/BulletPhysics)
add_subdirectory(Systems/GLWindow)
add_subdirectory(Systems/GLRender)
add_subdirectory(Runtimes/GLClient)
