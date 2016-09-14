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
		////////////////////////
		///   Constructors   ///
	public:

		Handle()
			: id{ NULL_COMPONENT }
		{
		}
		Handle(Handle<const C> copy)
			: id{ copy.id }
		{
		}
		
		//////////////////
		///   Fields   ///
	public:

		ComponentID id;

		///////////////////
		///   Methods   ///
	public:

		bool is_null() const
		{
			return id == NULL_COMPONENT;
		}
	};

	struct ComponentIdentity
	{
		ComponentID id;
		EntityID entity;
	};

	struct ComponentContext
	{
		ComponentID id;
		EntityID entity;
		const Scene* scene;
	};

	struct ComponentContextMut
	{
		ComponentID id;
		EntityID entity;
		Scene* scene;
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

		/////////////////////
		///   Operators   ///
	public:

		C* operator->() const
		{
			return object;
		}
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

	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(ComponentInstance<const C>))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = ComponentInstance<const C>{
				context->id,
				context->entity,
				self
			};

			return getter(instance);
		};
	}

	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(ComponentInstance<const C>, const Scene&))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = ComponentInstance<const C>{
				context->id,
				context->entity,
				self
			};

			return getter(instance, *context->scene);
		};
	}

	template <class C, typename PropT>
	auto component_setter(void(*setter)(ComponentInstance<C>, PropT))
	{
		return [setter](C* self, const ComponentContextMut* context, const PropT* value) {
			auto instance = ComponentInstance<C>{
				context->id,
				context->entity,
				self
			};

			setter(instance, *value);
		};
	}

	template <class C, typename PropT>
	auto component_setter(void(*setter)(ComponentInstance<C>, Scene&, PropT))
	{
		return [setter](C* self, const ComponentContextMut* context, const PropT* value) {
			auto instance = ComponentInstance<C>{
				context->id,
				context->entity,
				self
			};

			setter(instance, *context->scene, *value);
		};
	}
}
