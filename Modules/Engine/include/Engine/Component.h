// Component.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	struct Frame;

	/* Uniquely identifies an Entity within a Scene. */
	using EntityId = uint64;

	/* EntityId reserved for non-existant entities. */
	constexpr EntityId NULL_ENTITY = 0;

	/* EntityId reserved for the World entity. */
	constexpr EntityId WORLD_ENTITY = 1;

	/* Uniquely identifies a component within a scene. */
	struct SGE_ENGINE_API ComponentId
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentId()
			: _entity(NULL_ENTITY), _type(nullptr)
		{
		}
		ComponentId(EntityId entity, const TypeInfo& type)
			: _entity(entity), _type(&type)
		{
		}

		static ComponentId null()
		{
			return{};
		}

		///////////////////
		///   Methods   ///
	public:

		EntityId entity() const
		{
			return _entity;
		}

		const TypeInfo* type() const
		{
			return _type;
		}

		bool is_null() const
		{
			return _entity == NULL_ENTITY;
		}

		/////////////////////
		///   Operators   ///
	public:

		friend bool operator==(const ComponentId& lhs, const ComponentId& rhs)
		{
			return lhs._entity == rhs._entity && lhs._type == rhs._type;
		}
		friend bool operator!=(const ComponentId& lhs, const ComponentId& rhs)
		{
			return !(lhs == rhs);
		}

		//////////////////
		///   Fields   ///
	private:

		EntityId _entity;
		const TypeInfo* _type;
	};

	/* Represents an instance of a mutable component. */
	struct SGE_ENGINE_API ComponentInstanceMut
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstanceMut(ComponentId id, void* object)
			: _id(id), _object(object)
		{
		}

		static ComponentInstanceMut null()
		{
			return{ ComponentId::null(), nullptr };
		}

		///////////////////
		///   Methods   ///
	public:

		ComponentId id() const
		{
			return _id;
		}

		EntityId entity() const
		{
			return _id.entity();
		}

		const TypeInfo* type() const
		{
			return _id.type();
		}

		void* object() const
		{
			return _object;
		}

		bool is_null() const
		{
			return _id.is_null();
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentId _id;
		void* _object;
	};

	/* Represents an instance of an immutable component. */
	struct SGE_ENGINE_API ComponentInstance
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentInstance(ComponentId id, const void* object)
			: _id(id), _object(object)
		{
		}

		ComponentInstance(const ComponentInstanceMut& instance)
			: _id(instance.id()), _object(instance.object())
		{
		}

		static ComponentInstance null()
		{
			return{ ComponentId::null(), nullptr };
		}

		///////////////////
		///   Methods   ///
	public:

		ComponentId id() const
		{
			return _id;
		}

		const TypeInfo* type() const
		{
			return _id.type();
		}

		EntityId entity() const
		{
			return _id.entity();
		}

		const void* object() const
		{
			return _object;
		}

		bool is_null() const
		{
			return _id.is_null();
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentId _id;
		const void* _object;
	};

	template <class C>
	struct TComponentId : ComponentId
	{
		TComponentId()
			: ComponentId(NULL_ENTITY, sge::get_type<C>())
		{
		}
		TComponentId(EntityId entity)
			: ComponentId(entity, sge::get_type<C>())
		{
		}
	};

	/* Provides typed access to a mutable component. */
	template <class C>
	struct TComponentInstance : ComponentInstanceMut
	{
		using ComponentT = C;

		////////////////////////
		///   Constructors   ///
	public:

		TComponentInstance(EntityId entity, C* object)
			: ComponentInstanceMut(ComponentId{ entity, sge::get_type<C>() }, object)
		{
		}

		static TComponentInstance null()
		{
			return{ NULL_ENTITY, nullptr };
		}

		///////////////////
		///   Methods   ///
	public:

		TComponentId<C> id() const
		{
			return{ entity() };
		}

		C* object() const
		{
			return static_cast<C*>(ComponentInstanceMut::object());
		}

		/////////////////////
		///   Operators   ///
	public:

		C* operator->() const
		{
			return this->object();
		}
	};

	/* Provides typed access to an immutable component. */
	template <class C>
	struct TComponentInstance< const C > : ComponentInstance
	{
		using ComponentT = const C;

		////////////////////////
		///   Constructors   ///
	public:

		TComponentInstance(EntityId entity, const C* object)
			: ComponentInstance(ComponentId{ entity, sge::get_type<C>() }, object)
		{
		}

		TComponentInstance(const TComponentInstance<C>& instance)
			: ComponentInstance(instance.id(), instance.object())
		{
		}

		static TComponentInstance null()
		{
			return{ NULL_ENTITY, nullptr };
		}

		///////////////////
		///   Methods   ///
	public:

		TComponentId<C> id() const
		{
			return{ entity() };
		}

		const C* object() const
		{
			return static_cast<const C*>(ComponentInstance::object());
		}

		/////////////////////
		///   Operators   ///
	public:

		const C* operator->() const
		{
			return this->object();
		}
	};

	/* Context object supplied to component property getters and setters. */
	struct SGE_ENGINE_API ComponentContext
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentContext(ComponentId id)
			: _id(id), _frame(nullptr), _c_frame(nullptr)
		{
		}
		ComponentContext(ComponentId id, Frame* frame)
			: _id(id), _frame(frame), _c_frame(frame)
		{
		}
		ComponentContext(ComponentId id, const Frame* frame)
			: _id(id), _frame(nullptr), _c_frame(frame)
		{
		}

		ComponentContext(const ComponentContext& copy) = delete;
		ComponentContext& operator=(const ComponentContext& copy) = delete;

		///////////////////
		///   Methods   ///
	public:

		ComponentId id() const
		{
			return _id;
		}

		Frame* frame()
		{
			return _frame;
		}

		const Frame* frame() const
		{
			return _c_frame;
		}

		//////////////////
		///   Fields   ///
	private:

		ComponentId _id;
		Frame* _frame;
		const Frame* _c_frame;
	};

	/* Wraps a function expecting a component instance into a standard component getter function. */
	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(TComponentInstance<const C>))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = TComponentInstance<const C>{
				context->id().entity(),
				self
			};

			return getter(instance);
		};
	}

	/* Wraps a function expecting a component instance and a scene into a standard component getter function. */
	template <class C, typename PropT>
	auto component_getter(PropT(*getter)(TComponentInstance<const C>, const Frame&))
	{
		return [getter](const C* self, const ComponentContext* context) -> PropT {
			auto instance = TComponentInstance<const C>{
				context->id().entity(),
				self
			};

			return getter(instance, *context->frame());
		};
	}

	/* Wraps a function expecting a component instance into a standard component setter function. */
	template <class C, typename RetT, typename PropT>
	auto component_setter(RetT(*setter)(TComponentInstance<C>, PropT))
	{
		return [setter](C* self, ComponentContext* context, const PropT* value) {
			auto instance = TComponentInstance<C>{
				context->id().entity(),
				self
			};

			setter(instance, *value);
		};
	}

	/* Wraps a function expecting a component instance and a scene into a standard component setter function. */
	template <class C, typename RetT, typename PropT>
	auto component_setter(RetT(*setter)(TComponentInstance<C>, Frame&, PropT))
	{
		return [setter](C* self, ComponentContext* context, const PropT* value) {
			auto instance = TComponentInstance<C>{
				context->id().entity(),
				self
			};

			setter(instance, *context->frame(), *value);
		};
	}
}

namespace std
{
	template <>
	struct hash< sge::ComponentId >
	{
		std::size_t operator()(const sge::ComponentId& key) const
		{
			return hash<sge::EntityId>()(key.entity()) ^ hash<const sge::TypeInfo*>()(key.type()) << 1;
		}
	};

}
