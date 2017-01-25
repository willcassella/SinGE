// Component.h
#pragma once

#include <set>
#include <Core/Util/StringUtils.h>
#include <Core/Reflection/Reflection.h>
#include <Core/Interfaces/IToArchive.h>
#include <Core/Interfaces/IFromArchive.h>
#include "config.h"

namespace sge
{
	struct Scene;
	struct ProcessingFrame;

	template <class C>
	struct TComponentId;

	/* Uniquely identifies an Entity within a Scene. */
	using EntityId = uint64;

	/* EntityId reserved for non-existant entities. */
	constexpr EntityId NULL_ENTITY = 0;

	/* EntityId reserved for the World entity. */
	constexpr EntityId WORLD_ENTITY = 1;

	/**
	 * \brief Uniquely identifies a Component of any type within a Scene.
	 */
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

		template <class C>
		ComponentId(TComponentId<C> id)
			: _entity(id.entity()), _type(id.is_null() ? nullptr : &sge::get_type<C>())
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

	/**
	 * \brief Uniquely identifies a Component of a specific type within a Scene.
	 * \tparam C The type of Component this object identifies.
	 */
	template <class C>
	struct TComponentId
	{
		////////////////////////
		///   Constructors   ///
	public:

		TComponentId()
			: _entity(NULL_ENTITY)
		{
		}
		TComponentId(EntityId entity)
			: _entity(entity)
		{
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
			return &sge::get_type<C>();
		}

		bool is_null() const
		{
			return _entity == NULL_ENTITY;
		}

		//////////////////
		///   Fields   ///
	private:

		EntityId _entity;
	};

	class SGE_ENGINE_API ComponentInterface
	{
		////////////////////////
		///   Constructors   ///
	public:

		ComponentInterface(ProcessingFrame& pframe, EntityId entity);

		///////////////////
		///   Methods   ///
	public:

		EntityId entity() const
		{
			return _entity;
		}

		ComponentId id() const
		{
			return{ _entity, get_type() };
		}

		void from_property_archive(ArchiveReader& reader);

		virtual const TypeInfo& get_type() const = 0;

        void create_tag(const TypeInfo& tag_type, void* tag) const;

        template <typename T>
        void create_tag(T&& tag) const
        {
            this->create_tag(sge::get_type<T>(), &tag);
        }

	protected:

		ProcessingFrame& processing_frame() const
		{
			return *_pframe;
		}

		void apply_component_modified_tag();

        template <typename T>
        void checked_setter(const T& new_value, T& old_value)
        {
            if (new_value != old_value)
            {
                old_value = new_value;
                apply_component_modified_tag();
            }
        }

		//////////////////
		///   Fields   ///
	private:

		ProcessingFrame* _pframe;
		EntityId _entity;
		bool _applied_modified_tag;
	};

	template <class ComponentT>
	class TComponentInterface : public ComponentInterface
	{
		////////////////////////
		///   Constructors   ///
	public:

		TComponentInterface(ProcessingFrame& pframe, EntityId entity)
			: ComponentInterface(pframe, entity)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		TComponentId<ComponentT> id() const
		{
			return{ this->entity() };
		}
	};

	class SGE_ENGINE_API ComponentContainer
	{
		////////////////////////
		///   Constructors   ///
	public:

		virtual ~ComponentContainer() = default;

		///////////////////
		///   Methods   ///
	public:

		virtual void reset() = 0;

		virtual void to_archive(ArchiveWriter& writer, const std::set<EntityId>& est_instances) const = 0;

		virtual void from_archive(ArchiveReader& reader, std::set<EntityId>& est_instances) = 0;

		virtual void create_component(EntityId entity) = 0;

		virtual void remove_component(EntityId entity) = 0;

		virtual bool create_interface(ProcessingFrame& pframe, EntityId entity, void* addr) = 0;
	};

	/**
	 * \brief Tag applied to new component objects.
	 */
	struct SGE_ENGINE_API FNewComponent
	{
		SGE_REFLECTED_TYPE;
	};

	/**
	 * \brief Tag applied to destroyed component objects.
	 */
	struct SGE_ENGINE_API FDestroyedComponent
	{
		SGE_REFLECTED_TYPE;
	};

	/**
	 * \brief Tag applied to component objects that are modiified.
	 */
	struct SGE_ENGINE_API FModifiedComponent
	{
		SGE_REFLECTED_TYPE;
	};

	/**
	 * \brief Registers all builtin engine component types.
	 * \param scene The scene to register the component types with.
	 */
	SGE_ENGINE_API void register_builtin_components(Scene& scene);
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
