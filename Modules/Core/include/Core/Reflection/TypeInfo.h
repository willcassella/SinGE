// TypeInfo.h
#pragma once

#include <map>
#include <unordered_map>
#include "../config.h"

namespace sge
{
	struct TypeInfo;

	struct InterfaceInfo;

	struct PropertyInfo;

	struct FieldInfo;

	template <typename T>
	struct TypeInfoBuilder;

	struct SGE_CORE_API TypeInfo
	{
		struct SGE_CORE_API Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data();

			//////////////////
			///   Fields   ///
		public:

			std::string name;
			size_t size;
			size_t alignment;
			void(*init)(void* self);
			void(*copy_init)(void* self, const void* copy);
			void(*move_init)(void* self, void* move);
			void(*copy_assign)(void* self, const void* copy);
			void(*move_assign)(void* self, void* move);
			void(*drop)(void* self);
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

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
