// Component.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace singe
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

	template <typename T>
	struct Handle final
	{
		ComponentID id;
	};

	template <typename T>
	struct ComponentInstance
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstance(ComponentID id, EntityID entity, T* object)
			: id{ id }, entity{ entity }, object{ object }
		{
			// All done
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
		T* const object;
	};

	struct ENGINE_API IComponent
	{
		REFLECTED_INTERFACE;
		AUTO_IMPL_0(IComponent);
	};
}
