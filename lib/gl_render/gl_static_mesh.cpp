#include <stdint.h>

#include "lib/gl_render/gl_material.h"
#include "lib/gl_render/gl_static_mesh.h"

namespace sge {
namespace gl_render {
namespace gl_static_mesh {
void upload_static_mesh_vertex_data(
    GLuint vao,
    const GLuint vertex_buffers[],
    size_t num_vertices,
    const Vec3* vertex_positions,
    const HalfVec3* vertex_normals,
    const HalfVec3* vertex_tangents,
    const int8_t* vertex_bitangent_signs,
    const UHalfVec2* material_uv_coords,
    const UHalfVec2* lightmap_uv_coords
) {
  // Bind VAO
  glBindVertexArray(vao);

  // Upload vertex position data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[POSITION_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vec3), vertex_positions, GL_STATIC_DRAW);

  // Define vertex position specification
  glEnableVertexAttribArray(gl_material::POSITION_ATTRIB_LOCATION);
  glVertexAttribPointer(gl_material::POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), nullptr);

  // Upload normal buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[NORMAL_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(HalfVec3), vertex_normals, GL_STATIC_DRAW);

  // Define vertex normal specification
  glEnableVertexAttribArray(gl_material::NORMAL_ATTRIB_LOCATION);
  glVertexAttribPointer(gl_material::NORMAL_ATTRIB_LOCATION, 3, GL_SHORT, GL_TRUE, sizeof(HalfVec3), nullptr);

  // Upload tangent buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[TANGENT_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(HalfVec3), vertex_tangents, GL_STATIC_DRAW);

  // Define vertex tangent specification
  glEnableVertexAttribArray(gl_material::TANGENT_ATTRIB_LOCATION);
  glVertexAttribPointer(
      gl_material::TANGENT_ATTRIB_LOCATION, 3, GL_SHORT, GL_TRUE, sizeof(HalfVec3), nullptr
  );

  // Upload bitangent sign buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[BITANGENT_SIGN_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices, vertex_bitangent_signs, GL_STATIC_DRAW);

  // Define bitangent specification
  glEnableVertexAttribArray(gl_material::BITANGENT_SIGN_ATTRIB_LOCATION);
  glVertexAttribPointer(gl_material::BITANGENT_SIGN_ATTRIB_LOCATION, 1, GL_BYTE, GL_FALSE, 1, nullptr);

  // Upload material uv data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[MATERIAL_UV_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(UHalfVec2), material_uv_coords, GL_STATIC_DRAW);

  // Define vertex material uv specification
  glEnableVertexAttribArray(gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION);
  glVertexAttribPointer(
      gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION,
      2,
      GL_UNSIGNED_SHORT,
      GL_TRUE,
      sizeof(UHalfVec2),
      nullptr
  );

  // Upload lightmap uv data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[LIGHTMAP_UV_BUFFER_INDEX]);
  glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(UHalfVec2), lightmap_uv_coords, GL_STATIC_DRAW);

  // Define vertex lightmap uv specification
  glEnableVertexAttribArray(gl_material::LIGHTMAP_TEXCOORD_ATTRIB_LOCATION);
  glVertexAttribPointer(
      gl_material::LIGHTMAP_TEXCOORD_ATTRIB_LOCATION,
      2,
      GL_UNSIGNED_SHORT,
      GL_TRUE,
      sizeof(UHalfVec2),
      nullptr
  );

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void upload_static_mesh_elements(GLuint vao, GLuint ebo, size_t num_elements, const uint32_t* elements) {
  // Bind the vertex array object
  glBindVertexArray(vao);

  // Bind the element buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // Upload the data
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_elements * sizeof(uint32_t), elements, GL_STATIC_DRAW);

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
}  // namespace gl_static_mesh
}  // namespace gl_render
}  // namespace sge
