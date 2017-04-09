// SceneData.h
#pragma once

#include <memory>
#include <unordered_map>
#include <Core/Memory/Buffers/MultiStackBuffer.h>
#include "Component.h"
#include "Node.h"
#include "SceneMod.h"
#include "Lightmap.h"

namespace sge
{
	struct SGE_ENGINE_API SceneData
	{
		////////////////////////
		///   Constructors   ///
	public:

		SceneData()
			: new_node_channel(sizeof(ENewNode), 32),
			destroyed_node_channel(sizeof(EDestroyedNode), 32),
			node_local_transform_changed_channel(sizeof(ENodeTransformChanged), 32),
			node_world_transform_changed_channel(sizeof(ENodeTransformChanged), 32),
			node_root_changed_channel(sizeof(ENodeRootChangd), 32)
		{
		}
		SceneData(const SceneData& copy) = delete;
		SceneData& operator=(const SceneData& copy) = delete;
		SceneData(SceneData&& move) = delete;
		SceneData& operator=(SceneData&& move) = delete;

		//////////////////
		///   Fields   ///
	public:

		/* Component Data */
		std::unordered_map<const TypeInfo*, std::unique_ptr<ComponentContainer>> components;

        /* Node data */
		NodeId next_node_id = NodeId{ 1 };
        MultiStackBuffer node_buffer;
		std::vector<void*> free_buffs;
        std::map<NodeId, Node*> nodes;
        std::vector<NodeId> root_nodes;

        /* Scene modification data */
        std::vector<NodeRootMod> system_node_root_changes; // All nodes that had their roots modified during this system frame
        std::vector<NodeLocalTransformMod> system_node_local_transform_changes; // All nodes that had their transform modified during this system frame
        std::vector<Node*> system_new_nodes; // All nodes that were created during this system frame
		std::vector<Node*> system_destroyed_nodes; // All nodes that were destroyed during this system frame
		std::vector<Node*> update_modified_nodes; // All nodes that had their mod_state modified this update frame
        std::vector<NodeId> update_destroyed_nodes; // All nodes that were destroyed this update frame

		/* Node event channels */
		EventChannel new_node_channel;
		EventChannel destroyed_node_channel;
		EventChannel node_local_transform_changed_channel;
		EventChannel node_world_transform_changed_channel;
		EventChannel node_root_changed_channel;

		/* Lightmap data */
		std::string lightmap_data_path;
	};
}
