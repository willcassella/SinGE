// Shader.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../build.h"

namespace sge
{
    struct SGE_RESOURCE_API Shader
    {
        ///////////////////
        ///   Methods   ///
    public:

        void from_file(std::istream& file, std::size_t end);

        const std::string& source() const
        {
            return _source;
        }

        //////////////////
        ///   Fields   ///
    private:

        std::string _source;
    };

    struct VertexShader : Shader
    {
        SGE_REFLECTED_TYPE;
    };

    struct PixelShader : Shader
    {
        SGE_REFLECTED_TYPE;
    };
}
