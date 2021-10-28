cmake_minimum_required(VERSION 3.16)
project(
    SinGE
    VERSION 1.0
    DESCRIPTION "A C++ Game Engine"
)

# Set linux options
if (${UNIX})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -fvisibility=hidden")
	set(CMAKE_INSTALL_RPATH "$ORIGIN")
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	find_package(PkgConfig REQUIRED)
endif()

# Set GCC options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Force color output
    add_compile_options(-fdiagnostics-color=always)
endif()

# Set Clang options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Force color output
    add_compile_options(-fcolor-diagnostics)

    # Supress buggy warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
endif()

add_subdirectory(Modules/Core)
add_subdirectory(Modules/Resource)
add_subdirectory(Modules/Engine)
add_subdirectory(Systems/BulletPhysics)
add_subdirectory(Systems/GLWindow)
add_subdirectory(Systems/GLRender)
add_subdirectory(Runtimes/GLClient)
