// SceneData.h
#pragma once

#include <set>
#include <memory>
#include <unordered_map>
#include "Component.h"

namespace sge
{
	struct ComponentType
	{
		//////////////////
		///   Fields   ///
	public:

		/* All instances of this component which are recognized by processing functions. */
		std::set<EntityId> established_instances;

		/* The container for this component. */
		std::unique_ptr<ComponentContainer> container;
	};

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
		std::unordered_map<EntityId, EntityId> entity_parents;
		std::unordered_map<EntityId, std::string> entity_names;

		/* Component Data */
		std::unordered_map<const TypeInfo*, ComponentType> components;
	};
}
