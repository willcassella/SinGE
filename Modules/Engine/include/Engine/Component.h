// Component.h
#pragma once

#include <tuple>
#include <Core/STDE/TMP.h>
#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	struct Scene;

	/* Type used to represent Component Identifiers. */
	using ComponentID = uint64;

	/* Type used to represent Entities. */
	using EntityID = uint64;

	/* ComponentID used for non-existant components. */
	constexpr ComponentID NULL_COMPONENT = 0;

	/* EntityID used for non-existant entities. */
	constexpr EntityID NULL_ENTITY = 0;
	
	/* EntityID used for the World entity. */
	constexpr EntityID WORLD_ENTITY = 1;

	template <typename C>
	struct Handle
	{
		ComponentID id;
	};

	struct ComponentIdentity
	{
		ComponentID id;
		EntityID entity;
	};

	template <typename C>
	struct ComponentInstance
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstance(ComponentID id, EntityID entity, C* object)
			: id{ id }, entity{ entity }, object{ object }
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
	};

	namespace comp
	{
		template <typename C>
		struct With : ComponentInstance<C>
		{
			using ComponentInstance<C>::ComponentInstance;
		};

		template <typename C>
		struct Without
		{
		};

		template <typename C>
		struct Optional : ComponentInstance<C>
		{
			using ComponentInstance<C>::ComponentInstance;
		};
	}

	struct SGE_ENGINE_API TNewComponent
	{
		SGE_REFLECTED_TYPE;
	};
}
