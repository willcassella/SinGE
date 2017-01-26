// EditorOps.h
#pragma once

namespace sge
{
	struct Scene;
	struct SceneData;
	struct TypeDB;
	struct SystemFrame;
	class ArchiveWriter;
	class ArchiveReader;

	namespace editor_server
	{
		namespace ops
		{
			void get_type_info_query(const TypeDB& type_db, ArchiveReader& reader, ArchiveWriter& writer);

			void get_scene_query(const SystemFrame& frame, ArchiveWriter& writer);

			void get_component_types_query(const Scene& scene, ArchiveWriter& writer);

			void new_entity_query(SystemFrame& frame, ArchiveReader& reader);

			void destroy_entity_query(SystemFrame& frame, ArchiveReader& reader);

			void set_entity_name_query(SystemFrame& frame, ArchiveReader& reader);

			void set_entity_parent_query(SystemFrame& frame, ArchiveReader& reader);

			void new_component_query(SystemFrame& frame, ArchiveReader& reader);

			void destroy_component_query(SystemFrame& frame, ArchiveReader& reader);

			void get_component_query(SystemFrame& frame, ArchiveReader& reader, ArchiveWriter& writer);

			void set_component_query(SystemFrame& frame, ArchiveReader& reader);

			void get_resource_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

			void save_scene_query(const Scene& scene, ArchiveReader& reader);
		}
	}
}
