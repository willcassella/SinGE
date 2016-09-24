// TypeInfo.h
#pragma once

#include <map>
#include <unordered_map>
#include "PropertyInfo.h"
#include "FieldInfo.h"

namespace sge
{
	struct TypeInfo;

	struct InterfaceInfo;

	template <typename T>
	struct TypeInfoBuilder;

	struct TypeInfo
	{
		using InitFn = void(void* addr);
		using CopyInitFn = void(void* addr, const void* copy);
		using MoveInitFn = void(void* addr, void* move);
		using CopyAssignFn = void(void* self, const void* copy);
		using MoveAssignFn = void(void* self, void* move);
		using DropFn = void(void* self);

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data(std::string name)
				: name{ std::move(name) }
			{
				size = 0;
				alignment = 0;
				init = nullptr;
				copy_init = nullptr;
				move_init = nullptr;
				copy_assign = nullptr;
				move_assign = nullptr;
				drop = nullptr;
				base = nullptr;
			}

			//////////////////
			///   Fields   ///
		public:

			std::string name;
			size_t size;
			size_t alignment;
			InitFn* init;
			CopyInitFn* copy_init;
			MoveInitFn* move_init;
			CopyAssignFn* copy_assign;
			MoveAssignFn* move_assign;
			DropFn* drop;
			const TypeInfo* base;
			std::unordered_map<const InterfaceInfo*, const void*> interfaces;
			std::map<std::string, PropertyInfo> properties;
			std::map<std::string, FieldInfo> fields;
		};

		////////////////////////
		///   Constructors   ///
	public:

		TypeInfo(Data data)
			: _data(std::move(data))
		{
		}

		template <typename T>
		TypeInfo(TypeInfoBuilder<T>&& builder)
			: TypeInfo{ std::move(builder).result }
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/* Returns the name of this type. */
		const std::string& name() const
		{
			return _data.name;
		}

		/* Returns the size (in bytes) of this type. */
		std::size_t size() const
		{
			return _data.size;
		}

		/* Returns whether this type occupies any space. */
		bool is_empty() const
		{
			return size() == 0;
		}

		/* Returns the alignment requirements of this type. */
		std::size_t alignment() const
		{
			return _data.alignment;
		}

		/* Returns whether this type supports default-initialization. */
		bool has_init() const
		{
			return _data.init != nullptr;
		}

		/* Returns a pointer to the init function for this type. */
		InitFn* get_init() const
		{
			return _data.init;
		}

		/* Default-initializes an instance of this type at the given address. */
		void init(void* self) const
		{
			_data.init(self);
		}

		/* Returns whether this type supports copy-initialization. */
		bool has_copy_init() const
		{
			return _data.copy_init != nullptr;
		}

		/* Returns a pointer to the copy-init function for this type. */
		CopyInitFn* get_copy_init() const
		{
			return _data.copy_init;
		}

		/* Copy-initializes an instance of this type from the given instance, at the given addess. */
		void copy_init(void* self, const void* copy) const
		{
			return _data.copy_init(self, copy);
		}

		/* Returns whether this type supports move-initialization. */
		bool has_move_init() const
		{
			return _data.move_init != nullptr;
		}

		/* Returns a pointer to the move-init function for this type. */
		MoveInitFn* get_move_init() const
		{
			return _data.move_init;
		}

		/* Move-initializes an instance of this type from the given instance, at the given address. */
		void move_init(void* self, void* move) const
		{
			_data.move_init(self, move);
		}

		/* Returns whether this type supports copy-assignment. */
		bool has_copy_assign() const
		{
			return _data.copy_assign != nullptr;
		}

		/* Returns a pointer to the copy-assign function for this type. */
		CopyAssignFn* get_copy_assign() const
		{
			return _data.copy_assign;
		}

		/* Copy-assigns an instance of this type to the given instance. */
		void copy_assign(void* self, const void* copy) const
		{
			_data.copy_assign(self, copy);
		}

		/* Returns whether this type supports move-assignment. */
		bool has_move_assign() const
		{
			return _data.move_assign != nullptr;
		}

		/* Returns a pointer to the move-assign function for this type. */
		MoveAssignFn* get_move_assign() const
		{
			return _data.move_assign;
		}

		/* Move-assigns an instance of this tyep from the given instance. */
		void move_assign(void* self, void* move) const
		{
			_data.move_assign(self, move);
		}

		/* Returns whether this type supports dropping. */
		bool has_drop() const
		{
			return _data.drop != nullptr;
		}

		/* Returns a pointer to the drop function for this type. */
		DropFn* get_drop() const
		{
			return _data.drop;
		}

		/* Drops an instance of this type. */
		void drop(void* self) const
		{
			_data.drop(self);
		}

		/* Returns the base-type of this type (if one exists). */
		const TypeInfo* get_base() const
		{
			return _data.base;
		}

		/* Returns an iterator at the first property of this type.
		* NOTE: This does not include base class properties. */
		auto properties_begin() const
		{
			return _data.properties.begin();
		}

		/* Returns an iterator after the last property of this type. */
		auto properties_end() const
		{
			return _data.properties.end();
		}

		/* Searches for a property on this type with the given name.
		* Returns a pointer to the property if one was found, returns 'null' otherwise. */
		const PropertyInfo* find_property(const char* name) const
		{
			auto prop = _data.properties.find(name);

			if (prop == _data.properties.end())
			{
				return nullptr;
			}

			return &prop->second;
		}

		/* Searches for a property on this type with the given name, and sets the value.
		* Returns 'true' if the property was found and set, 'false' otherwise. */
		bool set_property(const char* name, void* object, void* context, const void* value) const
		{
			auto prop = _data.properties.find(name);

			if (prop == _data.properties.end() || prop->second.is_read_only())
			{
				return false;
			}

			prop->second.set(object, context, value);
			return true;
		}

		/* Searches for ap property on this type with the given name,and gets the value.
		* Returns 'true' of the property was found and gotten, 'false' otherwise. */
		bool get_property(const char* name, const void* object, const void* context, PropertyInfo::GetterOutFn out) const
		{
			auto prop = _data.properties.find(name);

			if (prop == _data.properties.end())
			{
				return false;
			}

			prop->second.get(object, context, out);
			return true;
		}

		/* Searches for the given property name, and runs the given function in between the getter and setter.
		* Returns 'true' if the property was found and mutated, 'false' otherwise. */
		bool mutate_property(const char* name, void* object, void* context, PropertyInfo::MutatorFn mutator) const
		{
			auto prop = _data.properties.find(name);

			if (prop == _data.properties.end() || prop->second.is_read_only())
			{
				return false;
			}

			prop->second.mutate(object, context, mutator);
			return true;
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
