// EditorOps.h
#pragma once

namespace sge
{
	struct Scene;
	struct SceneData;
	struct TypeDB;
	class ArchiveWriter;
	class ArchiveReader;

	namespace editor_server
	{
		namespace ops
		{
			void get_type_info_query(const TypeDB& type_db, ArchiveReader& reader, ArchiveWriter& writer);

			void get_scene_query(Scene& scene, ArchiveWriter& writer);

			void get_component_types_query(const Scene& scene, ArchiveWriter& writer);

			void new_node_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void destroy_node_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void node_name_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void node_root_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void node_local_transform_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void new_component_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void destroy_component_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void component_property_update_query(Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void get_resource_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void save_scene_query(const Scene& scene, ArchiveReader& reader);

            void generate_lightmaps(Scene& scene);
		}
	}
}
