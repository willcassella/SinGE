#pragma once

#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    struct SGE_RESOURCE_API Shader
    {
        void from_file(std::istream& file, size_t end);

        const std::string& source() const
        {
            return _source;
        }

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
