#include <iostream>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/resource/resources/shader.h"

SGE_REFLECT_TYPE(sge::VertexShader).flags(TF_SCRIPT_NOCONSTRUCT);

SGE_REFLECT_TYPE(sge::PixelShader).flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge {
void Shader::from_file(std::istream& file, size_t end) {
  // Read the file
  _source = std::string(end, ' ');
  file.read(&_source[0], end);
}
}  // namespace sge
