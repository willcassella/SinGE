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

	struct ComponentIdentity
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentIdentity()
			: id{ NULL_COMPONENT }, entity{ NULL_ENTITY }
		{
		}
		ComponentIdentity(ComponentID id, EntityID entity)
			: id{ id }, entity{ entity }
		{
		}

		//////////////////
		///   Fields   ///
	public:

		ComponentID id;
		EntityID entity;
	};

	template <typename C>
	struct ComponentInstance
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstance(ComponentIdentity identity, C* object)
			: _identity{ identity }, _object{ object }
		{
		}

		ComponentInstance(const ComponentInstance<std::remove_const_t<C>>& instance)
			: _identity{ instance.identity() }, _object{ instance.object() }
		{
		}

		static ComponentInstance null()
		{
			return{ ComponentIdentity{ NULL_COMPONENT, NULL_ENTITY }, nullptr };
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentIdentity _identity;
		C* _object;

		///////////////////
		///   Methods   ///
	public:

		ComponentIdentity identity() const
		{
			return _identity;
		}

		ComponentID id() const
		{
			return _identity.id;
		}

		EntityID entity() const
		{
			return _identity.entity;
		}

		C* object() const
		{
			return _object;
		}

		/////////////////////
		///   Operators   ///
	public:

		C* operator->() const
		{
			return _object;
		}
	};

	template <typename C>
	struct Handle
	{
		////////////////////////
		///   Constructors   ///
	public:

		Handle()
			: _id{ NULL_COMPONENT }
		{
		}
		Handle(const Handle<const C>& copy)
			: _id{ copy.id() }
		{
		}
		Handle(ComponentInstance<std::remove_const_t<C>> instance)
			: _id{ instance.id() }
		{
		}
		Handle(ComponentInstance<const C> instance)
			: _id{ instance.id() }
		{
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentID _id;

		///////////////////
		///   Methods   ///
	public:

		ComponentID id() const
		{
			return _id;
		}

		bool is_null() const
		{
			return id() == NULL_COMPONENT;
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

	/* Tag applied to new components. */
	struct SGE_ENGINE_API TNewComponent
	{
		SGE_REFLECTED_TYPE;
	};

	/* Context object supplied to component property getters and setters. */
	struct SGE_ENGINE_API ComponentContext
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentContext(ComponentIdentity identity)
			: _identity{ identity }, _scene{ nullptr }, _c_scene{ nullptr }
		{
		}
		ComponentContext(ComponentIdentity identity, Scene* scene)
			: _identity{ identity }, _scene{ scene }, _c_scene{ scene }
		{
		}
		ComponentContext(ComponentIdentity identity, const Scene* scene)
			: _identity{ identity }, _scene{ nullptr }, _c_scene{ scene }
		{
		}

		ComponentContext(const ComponentContext& copy) = delete;
		ComponentContext& operator=(const ComponentContext& copy) = delete;

		///////////////////
		///   Methods   ///
	public:

		ComponentIdentity identity() const
		{
			return _identity;
		}

		Scene* scene()
		{
			return _scene;
		}

		const Scene* scene() const
		{
			return _c_scene;
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentIdentity _identity;
		Scene* _scene;
		const Scene* _c_scene;
	};

	/* Wraps a function expecting a component instance into a standard component getter function. */
	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(ComponentInstance<const C>))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = ComponentInstance<const C>{
				context->identity(),
				self
			};

			return getter(instance);
		};
	}

	/* Wraps a function expecting a component instance and a scene into a standard component getter function. */
	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(ComponentInstance<const C>, const Scene&))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = ComponentInstance<const C>{
				context->identity(),
				self
			};

			return getter(instance, *context->scene());
		};
	}

	/* Wraps a function expecting a component instance into a standard component setter function. */
	template <class C, typename RetT, typename PropT>
	auto component_setter(RetT(*setter)(ComponentInstance<C>, PropT))
	{
		return [setter](C* self, ComponentContext* context, const PropT* value) {
			auto instance = ComponentInstance<C>{
				context->identity(),
				self
			};

			setter(instance, *value);
		};
	}

	/* Wraps a function expecting a component instance and a scene into a standard component setter function. */
	template <class C, typename RetT, typename PropT>
	auto component_setter(RetT(*setter)(ComponentInstance<C>, Scene&, PropT))
	{
		return [setter](C* self, ComponentContext* context, const PropT* value) {
			auto instance = ComponentInstance<C>{
				context->identity(),
				self
			};

			setter(instance, *context->scene(), *value);
		};
	}
}
