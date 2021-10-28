enable_language(CXX)
set(SOURCE_FILES
    source/Interfaces/IFromArchive.cpp
    source/Interfaces/IFromString.cpp
    source/Interfaces/IToArchive.cpp
    source/Interfaces/IToString.cpp
    source/Math/Angle.cpp
    source/Math/Mat4.cpp
    source/Math/Quat.cpp
    source/Math/Vec2.cpp
    source/Math/Vec3.cpp
    source/Math/Vec4.cpp
    source/Memory/Buffers/MultiStackBuffer.cpp
    source/Memory/Functions.cpp
    source/Reflection/EnumTypeInfo.cpp
    source/Reflection/Reflection.cpp
    source/Reflection/TypeDB.cpp
)
add_library(Core SHARED ${SOURCE_FILES})
target_compile_definitions(Core PRIVATE SGE_CORE_BUILD)
target_include_directories(Core PUBLIC include)
