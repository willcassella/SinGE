#pragma once

#include "lib/engine/components/display/spot_light.h"
#include "lib/gl_render/render_commands.h"

namespace sge {
struct CStaticMesh;

namespace gl_render {
struct RenderResource;

struct RenderScene_Lightmap {
  GLuint x_basis_tex = 0;
  GLuint y_basis_tex = 0;
  GLuint z_basis_tex = 0;
  GLuint direct_mask_tex = 0;
};

struct RenderScene_Mesh {
  /**
   * \brief Command object for the mesh to render.
   */
  RenderCommand_Mesh mesh_command;

  /**
   * \brief Command objects for each instance to be rendered.
   */
  std::vector<RenderCommand_MeshInstance> instance_commands;

  /**
   * \brief Array symmetrical with 'instance_commands', stores the ids for each node.
   */
  std::vector<NodeId> node_ids;
};

struct RenderScene_Material {
  gl_material::Material material;
  std::vector<RenderScene_Mesh> mesh_instances;
  std::map<GLuint, size_t> mesh_indices;
};

struct RenderScene_Spotlight {
  NodeId node_id;
  Mat4 view_matrix;
  Mat4 proj_matrix;
  GLuint shadow_framebuffer = 0;
  GLuint shadow_depthbuffer = 0;
  GLuint shadow_width = 0;
  GLuint shadow_height = 0;
};

struct RenderScene_LightmaskVolume {
  NodeId node_id;
  GLuint pos_buffer = 0;
  GLuint normal_buffer = 0;
  GLuint texcoord_buffer = 0;
  RenderCommand_Mesh volume_mesh;
  RenderCommand_MeshInstance mesh_instance;
};

struct RenderScene_LightmaskObject {
  NodeId node_id;
  gl_material::Material material;
  RenderCommand_Mesh mesh;
  RenderCommand_MeshInstance mesh_instance;
};

struct RenderScene_Commands {
  std::vector<RenderScene_Spotlight> spotlights;

  /**
   * \brief Rendering commands for all standard path objects.
   */
  std::vector<RenderScene_Material> standard_path_material_instances;

  std::map<GLuint, size_t> standard_path_material_indices;

  std::vector<RenderScene_LightmaskVolume> lightmask_volume_mesh_instances;

  std::vector<RenderScene_LightmaskObject> lightmask_receiver_mesh_instances;

  std::vector<RenderScene_LightmaskObject> lightmask_occluder_mesh_instances;

  /**
   * \brief Mapping between objects and their lightmaps.
   */
  std::map<NodeId, RenderScene_Lightmap> node_lightmaps;

  /* Lightmap data */
  Vec3 light_dir;
  color::RGBF32 light_intensity;
};

void RenderScene_render(
    const RenderScene_Commands& commands,
    const RenderResource& resources,
    const GLuint gbuffer,
    const GLuint gbuffer_width,
    const GLuint gbuffer_height,
    const Mat4& view_matrix,
    const Mat4& proj_matrix
);

void RenderScene_update_matrices(
    RenderScene_Commands& commands,
    const NodeId* const nodes_ids,
    const Mat4* const matrices,
    const size_t num_nodes
);

void RenderScene_insert_static_mesh_commands(
    RenderScene_Commands& commands,
    RenderResource& resources,
    const Node* const* const nodes,
    const CStaticMesh* const* const static_meshes,
    const size_t num_static_meshes
);

void RenderScene_remove_static_mesh_commands(
    RenderScene_Commands& commands,
    const NodeId* const node_ids,
    const size_t num_nodes
);

void RenderScene_insert_spotlight_commands(
    RenderScene_Commands& commands,
    RenderResource& resources,
    const Node* const* const nodes,
    const CSpotlight* const* const spotlights,
    const size_t num_spotlights
);

void RenderScene_update_spotlight_commands(
    RenderScene_Commands& commands,
    RenderResource& resources,
    const Node* const* const nodes,
    const CSpotlight* const* const spotlights,
    const size_t num_spotlights
);

void RenderScene_remove_spotlight_commands(
    RenderScene_Commands& commands,
    NodeId* const node_ids,
    size_t num_nodes
);

void RenderScene_clear(RenderScene_Commands& commands);
}  // namespace gl_render
}  // namespace sge
