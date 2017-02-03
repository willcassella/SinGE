// Component.h
#pragma once

#include <Core/Util/StringUtils.h>
#include <Core/Reflection/Reflection.h>
#include <Core/Interfaces/IToArchive.h>
#include <Core/Interfaces/IFromArchive.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "config.h"

namespace sge
{
	struct Scene;
    struct TagBuffer;
    struct SystemFrame;
	struct ProcessingFrame;
    class ComponentInterface;

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
        friend bool operator<(const ComponentId& lhs, const ComponentId& rhs)
        {
            return lhs._type < rhs._type || (lhs._type == rhs._type && lhs._entity < rhs._entity);
        }
	    friend bool operator>(const ComponentId& lhs, const ComponentId& rhs)
        {
            return lhs._type > rhs._type || (lhs._type == rhs._type && lhs._entity > rhs._entity);
        }
        friend bool operator<=(const ComponentId& lhs, const ComponentId& rhs)
        {
            return lhs < rhs || lhs == rhs;
        }
        friend bool operator>=(const ComponentId& lhs, const ComponentId& rhs)
        {
            return lhs > rhs || lhs == rhs;
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
        SGE_REFLECTED_TYPE;

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

    class SGE_ENGINE_API ComponentContainer
    {
    public:

        using InstanceIterator = const EntityId*;

        ////////////////////////
        ///   Constructors   ///
    public:

        virtual ~ComponentContainer() = default;

        ///////////////////
        ///   Methods   ///
    public:

        virtual void reset() = 0;

        virtual void to_archive(ArchiveWriter& writer) const = 0;

        virtual void from_archive(ArchiveReader& reader) = 0;

        virtual InstanceIterator get_start_iterator() const = 0;

        virtual InstanceIterator get_end_iterator() const = 0;

        virtual void create_instances(const EntityId* ord_entities, std::size_t num, std::size_t num_dups) = 0;

        virtual void remove_instances(const EntityId* ord_instances, std::size_t num) = 0;

        virtual void reset_interface(InstanceIterator instance, ComponentInterface* interf) = 0;
    };

    /**
	 * \brief Provides access to a component within a processing frame.
	 */
	class SGE_ENGINE_API ComponentInterface
	{
        friend SystemFrame;

		////////////////////////
		///   Constructors   ///
	public:

		ComponentInterface();
        virtual ~ComponentInterface() = default;

		///////////////////
		///   Methods   ///
	public:

		EntityId entity() const
		{
            return *_iter;
		}

        ComponentId id() const;

		virtual const TypeInfo& get_type() const = 0;

		void from_property_archive(ArchiveReader& reader);

        void destroy();

	protected:

        virtual void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags);

        void set_modified();

        template <typename T>
        void checked_setter(const T& new_value, T& old_value)
        {
            if (new_value != old_value)
            {
                old_value = new_value;
                set_modified();
            }
        }

	private:

        void reset(ComponentContainer::InstanceIterator iter);

	    //////////////////
		///   Fields   ///
	private:

        ComponentContainer::InstanceIterator _iter;
        std::vector<EntityId> _ord_modified;
        std::vector<EntityId> _ord_destroyed;
		bool _modified_current;
        bool _destroyed_current;
	};

	template <class ComponentT>
	class TComponentInterface : public ComponentInterface
	{
		///////////////////
		///   Methods   ///
	public:

		TComponentId<ComponentT> id() const
		{
			return{ this->entity() };
		}
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

template <class C>
SGE_REFLECT_TYPE_TEMPLATE(sge::TComponentId, C);
