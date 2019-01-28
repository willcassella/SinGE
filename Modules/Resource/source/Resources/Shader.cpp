// Shader.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Resource/Resources/Shader.h"

SGE_REFLECT_TYPE(sge::VertexShader)
.flags(TF_SCRIPT_NOCONSTRUCT);

SGE_REFLECT_TYPE(sge::PixelShader)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
    void Shader::from_file(std::istream& file, std::size_t end)
    {
        // Read the file
        _source = std::string(end, ' ');
        file.read(&_source[0], end);
    }
}
