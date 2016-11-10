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

		ComponentInterface(ProcessingFrame& pframe, EntityId entity)
			: _pframe(&pframe),
			_entity(entity)
		{
		}

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

		virtual const TypeInfo& get_type() const = 0;

		virtual void from_archive(const ArchiveReader& reader);

	protected:

		ProcessingFrame& processing_frame() const
		{
			return *_pframe;
		}

		//////////////////
		///   Fields   ///
	private:

		ProcessingFrame* _pframe;
		EntityId _entity;
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

		virtual void to_archive(ArchiveWriter& writer) const = 0;

		virtual void from_archive(const ArchiveReader& reader) = 0;

		virtual void create_component(EntityId entity) = 0;

		virtual bool create_interface(ProcessingFrame& pframe, EntityId entity, void* addr) = 0;

		//////////////////
		///   Fields   ///
	public:

		std::set<EntityId> entities;
	};

	template <class ComponentT, typename ComponentDataT>
	class BasicComponentContainer final : public ComponentContainer
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const override
		{
			for (const auto& instance : instances)
			{
				writer.add_object_member(sge::to_string(instance.first).c_str(),
					[&value = instance.second](ArchiveWriter& instanceWriter)
				{
					sge::to_archive(value, instanceWriter);
				});
			}
		}

		void from_archive(const ArchiveReader& reader) override
		{
			reader.enumerate_object_members([this](const char* entityIdStr, const auto& instanceReader)
			{
				EntityId entity = std::strtoull(entityIdStr, nullptr, 10);

				// See if this instance already exists
				auto iter = this->instances.find(entity);
				if (iter == this->instances.end())
				{
					// If not, create one
					iter = instances.insert(std::make_pair(entity, ComponentDataT{})).first;
					entities.insert(entity);
				}

				sge::from_archive(iter->second, instanceReader);
			});
		}

		void create_component(EntityId entity) override
		{
			instances.insert(std::make_pair(entity, ComponentDataT{}));
			entities.insert(entity);
		}

		bool create_interface(ProcessingFrame& pframe, EntityId entity, void* addr) override
		{
			auto data = instances.find(entity);
			if (data == instances.end())
			{
				return false;
			}

			new (addr) ComponentT{ pframe, entity, data->second };
			return true;
		}

		//////////////////
		///   Fields   ///
	public:

		std::unordered_map<EntityId, ComponentDataT> instances;
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
