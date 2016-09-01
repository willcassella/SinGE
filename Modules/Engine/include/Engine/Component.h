// Component.h
#pragma once

#include <tuple>
#include <Core/STDE/TMP.h>
#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	/* Type used to represent Components. */
	using ComponentID = uint64;

	/* Type used to represent Entities. */
	using EntityID = uint64;

	/* ComponentID used for non-existant components. */
	constexpr ComponentID NULL_COMPONENT = 0;

	/* EntityID used for non-existant entities. */
	constexpr EntityID NULL_ENTITY = 0;
	
	/* EntityID used for the World entity. */
	constexpr EntityID WORLD_ENTITY = 1;

	struct ComponentIdentity
	{
		EntityID entity;
		ComponentID component;
	};

	template <typename C>
	struct Handle
	{
		ComponentID id;
	};

	template <typename C, typename ... Tags>
	struct ComponentInstance
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstance(ComponentID id, EntityID entity, C* object)
			: id{ id }, entity{ entity }, object{ object }
		{
		}

		ComponentInstance(ComponentID id, EntityID entity, C* object, std::tuple<Tags...> tags)
			: id{ id }, entity{ entity }, object{ object }, tags{ std::move(tags) }
		{
		}

		static ComponentInstance null()
		{
			return{ NULL_COMPONENT, NULL_ENTITY, nullptr };
		}

		//////////////////
		///   Fields   ///
	public:

		const ComponentID id;
		const EntityID entity;
		C* const object;
		const std::tuple<std::pair<const Tags*, int>...> tags;
	};

	template <typename C, typename ... Tags>
	struct RequiredComponent : ComponentInstance<C, Tags...>
	{
	};

	template <typename C, typename ... Tags>
	struct OptionalComponent : ComponentInstance<C, Tags...>
	{
	};

	template <typename C, typename ... Tags>
	struct RequiredNotComponent
	{
	};

	struct SGE_ENGINE_API TNewComponent
	{
		SGE_REFLECTED_TYPE;
	};
}
