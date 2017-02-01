// SceneData.h
#pragma once

#include <memory>
#include <unordered_map>
#include "Component.h"

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
	};
}
