// SceneData.h
#pragma once

#include <memory>
#include <unordered_map>
#include <Core/Memory/Buffers/MultiStackBuffer.h>
#include "Component.h"
#include "Node.h"
#include "SceneMod.h"

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
			node_transform_changed_channel(sizeof(ENodeTransformChanged), 32),
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
        std::map<NodeId, Node*> nodes;
        std::vector<NodeId> root_nodes;

        /* Scene modification data */
        std::vector<Node*> new_nodes;
        std::vector<Node*> destroyed_nodes;
		std::vector<Node*> modified_nodes;
        std::vector<NodeTransformMod> node_transform_changes;
        std::vector<NodeRootMod> node_root_changes;

		/* Node event channels */
		EventChannel new_node_channel;
		EventChannel destroyed_node_channel;
		EventChannel node_transform_changed_channel;
		EventChannel node_root_changed_channel;
	};
}
