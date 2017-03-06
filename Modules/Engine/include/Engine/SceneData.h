// SceneData.h
#pragma once

#include <memory>
#include <unordered_map>
#include <Core/Memory/Buffers/MultiStackBuffer.h>
#include "Component.h"
#include "Node.h"

namespace sge
{
	struct SGE_ENGINE_API SceneData
	{
		////////////////////////
		///   Constructors   ///
	public:

		SceneData() = default;
		SceneData(const SceneData& copy) = delete;
		SceneData& operator=(const SceneData& copy) = delete;
		SceneData(SceneData&& move) = default;
		SceneData& operator=(SceneData&& move) = default;

		//////////////////
		///   Fields   ///
	public:

		/* When a new entity is created, this is the Id it will be given. */
		EntityId next_entity_id;

		/* Entity Data */
		std::unordered_map<EntityId, std::string> entity_names;
		std::unordered_map<EntityId, EntityId> entity_parents;
        std::unordered_map<EntityId, std::vector<EntityId>> entity_children;

		/* Component Data */
		std::unordered_map<const TypeInfo*, std::unique_ptr<ComponentContainer>> components;

        /* Node data */
        Node::Id next_node_id = 1;
        MultiStackBuffer node_buffer;
        std::map<Node::Id, Node*> nodes;
        std::vector<Node::Id> root_nodes;
	};
}
