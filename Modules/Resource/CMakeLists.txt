enable_language(CXX)
set(SOURCE_FILES
    source/Archives/BinaryArchive.cpp
    source/Archives/JsonArchive.cpp
    source/Interfaces/IFromFile.cpp
    source/Misc/Color.cpp
    source/Misc/ImageOps.cpp
    source/Misc/LightmaskVolume.cpp
    source/ResourceManager.cpp
    source/Resources/HDRImage.cpp
    source/Resources/Image.cpp
    source/Resources/Material.cpp
    source/Resources/Shader.cpp
    source/Resources/StaticMesh.cpp
    source/Resources/Texture.cpp
)
add_library(Resource SHARED ${SOURCE_FILES})
target_compile_definitions(Resource PRIVATE SGE_RESOURCE_BUILD)
target_include_directories(Resource PUBLIC include)
target_include_directories(Resource PRIVATE private)
target_link_libraries(Resource PUBLIC Core)
target_link_libraries(Resource PRIVATE freeimage)
