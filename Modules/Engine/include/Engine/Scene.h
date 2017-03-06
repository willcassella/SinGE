// Scene.h
#pragma once

#include "SceneData.h"

namespace sge
{
	struct TypeDB;
    struct UpdatePipeline;

	/**
	 * \brief Top-level scene interface.
	 */
	struct SGE_ENGINE_API Scene
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Scene(TypeDB& typedb);
		Scene(const Scene& copy) = delete;
		Scene& operator=(const Scene& copy) = delete;
		Scene(Scene&& move) = default;
		Scene& operator=(Scene&& move) = default;
		~Scene();

		///////////////////
		///   Methods   ///
	public:

        Node* create_node();

        /**
        * \brief Searches for a node with the given Id, and returns it.
        * \param id The Id of the node to search for.
        * \return A pointer to the node. This pointer will no longer be valid at the end of the scene update.
        */
        Node* get_node(Node::Id id);

	    /**
         * \brief Searches for a node with the given Id, and returns it.
         * \param id The Id of the node to search for.
         * \return A pointer to the node. This pointer will no longer be valid at the end of the scene update.
         */
        const Node* get_node(Node::Id id) const;

	    /**
         * \brief Constructs an array of all root nodes in the scene.
         * \param out_roots The array to fill with the root nodes.
         */
        void get_root_nodes(std::vector<Node::Id>* out_roots) const;

		/**
		 * \brief Resets entity/component data. Essentially creates a new scene without unregistering component types.
		 */
		void reset_scene();

		/**
		 * \brief Returns raw scene data. You should not use this unless you know what you're doing.
		 */
		const SceneData& get_raw_scene_data() const;

		/**
		 * \brief Serializes the state of this Scene to an Archive.
		 * \param writer The writer for the archive to serialize to.
		 */
		void to_archive(ArchiveWriter& writer) const;

		/**
		 * \brief Deserializes the state of this Scene from an Archive.
		 * \param reader The reader for the Archive to deserialize from.
		 */
		void from_archive(ArchiveReader& reader);

		/**
		 * \brief Returns the type database for this Scene.
		 */
		TypeDB& get_type_db();

		/**
		 * \brief Returns the type database for this Scene.
		 */
		const TypeDB& get_type_db() const;

		/**
		 * \brief Searches for a component type with the given name in the type database.
		 * \param typeName The name of the component type to search for.
		 * \return A pointer to the type information for the type, if found.
		 */
		const TypeInfo* get_component_type(const char* typeName) const;

		/**
		 * \brief Registers a new component type with the scene.
		 * \param type
		 * \param container
		 */
		void register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container);

		/**
		 * \brief Runs a full update of the scene, using the given update pipeline and time delta.
		 * \param pipeline The update pipeline to use to update the scene.
		 * \param dt The game time that is supposed to have passed since the last call to 'update'.
		 */
		void update(UpdatePipeline& pipeline, float dt);

	private:

        std::vector<SystemFrame> apply_changes(
            UpdatePipeline& pipeline,
            SystemFrame* frames,
            std::size_t num_frames);

		//////////////////
		///   Fields   ///
	private:

		TypeDB* _type_db;
		float _current_time;
        SceneData _scene_data;
	};
}
