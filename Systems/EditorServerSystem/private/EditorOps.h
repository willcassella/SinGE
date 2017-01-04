// EditorOps.h
#pragma once

namespace sge
{
	struct Scene;
	struct TypeInfo;
	class ArchiveWriter;
	class ArchiveReader;

	namespace editor_ops
	{
		/**
		 * \brief
		 * \param scene
		 * \param reader
		 * \param writer
		 */
		void get_type_info_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

		/**
		 * \brief Returns the structure of the scene, including entities and what components are attached to those entities.
		 * \param scene The scene to gather information from.
		 * \param writer The output device.
		 */

		void get_component_types_query(const Scene& scene, ArchiveWriter& writer);

		void new_entity_query(Scene& scene, ArchiveReader& reader);

		void set_entity_name_query(Scene& scene, ArchiveReader& reader);

		void set_entity_parent_query(Scene& scene, ArchiveReader& reader);

		void new_component_query(Scene& scene, ArchiveReader& reader);

		void get_component_query(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);

		void set_component_query(Scene& scene, ArchiveReader& reader);

		void get_scene_query(const Scene& scene, ArchiveWriter& writer);

		void get_resource(const Scene& scene, ArchiveReader& reader, ArchiveWriter& writer);
	}
}
