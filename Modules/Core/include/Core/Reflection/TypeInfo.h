// TypeInfo.h
#pragma once

#include <cstdint>
#include <string>
#include <typeinfo>
#include "../Functional/FunctionView.h"

namespace sge
{
	struct InterfaceInfo;
	struct ConstructorInfo;
	struct PropertyInfo;
	struct FieldInfo;

	enum TypeFlags
	{
		/**
		 * \brief This type has no special flags.
		 */
		TF_NONE = 0,

		/**
		 * \brief This type is a native (C++) type.
		 */
		TF_NATIVE = (1 << 0),

		/**
		 * \brief This type is a native (C++) primitive type.
		 */
		TF_PRIMITIVE = (1 << 1) | TF_NATIVE,

		/**
		 * \brief This type may not be constructed by scripts.
		 */
		TF_SCRIPT_NOCONSTRUCT = (1 << 2) | TF_NATIVE,
	};

	struct TypeInfo
	{
		using ConstructorEnumeratorFn = void(std::size_t argc, const ConstructorInfo& ctor);
		using NamedConstructorEnumeratorFn = void(const std::string& name, const ConstructorInfo& ctor);
		using PropertyEnumeratorFn = void(const std::string& name, const PropertyInfo& prop);
		using FieldEnumeratorFn = void(const std::string& name, const FieldInfo& field);

		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data(std::string name)
				: flags(TF_NONE), name(std::move(name)), size(0), alignment(0), base(nullptr), native_type_info(nullptr)
			{
			}

			//////////////////
			///   Fields   ///
		public:

			TypeFlags flags;
			std::string name;
			std::size_t size;
			std::size_t alignment;
			const TypeInfo* base;
			const std::type_info* native_type_info;
		};

		////////////////////////
		///   Constructors   ///
	public:

		explicit TypeInfo(Data data)
			: _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/**
		 * \brief Returns the flags associated with this type.
		 */
		TypeFlags flags() const
		{
			return _data.flags;
		}

		/**
		 * \brief Returns the name of this type.
		 */
		const std::string& name() const
		{
			return _data.name;
		}

		/**
		 * \brief Returns the size (in bytes) of this type.
		 */
		std::size_t size() const
		{
			return _data.size;
		}

		/**
		 * \brief Returns the alignment requirements of this type.
		 */
		std::size_t alignment() const
		{
			return _data.alignment;
		}

		/**
		 * \brief Returns a pointer to the base type (if applicable) of this type.
		 */
		const TypeInfo* base() const
		{
			return _data.base;
		}

		/**
		 * \brief Returns whether this type occupies any space.
		 */
		bool is_empty() const
		{
			return size() == 0;
		}

		/**
		 * \brief Returns whether this type is a native (C++) type.
		 */
		bool is_native() const
		{
			return (_data.flags & TF_NATIVE) != 0;
		}

		/**
		 * \brief Returns whether this type is a native (C++) primitive type.
		 */
		bool is_primitive() const
		{
			return (_data.flags & TF_PRIMITIVE) != 0;
		}

		/**
		 * \brief Returns whether this type supports default-initializiation.
		 */
		virtual bool has_init() const = 0;

		/**
		 * \brief Initializes an instance of this type.
		 * \param addr The address to initialize the instance at.
		 */
		virtual void init(void* addr) const = 0;

		/**
		 * \brief Returns whether this type supports copy-initialization.
		 */
		virtual bool has_copy_init() const = 0;

		/**
		 * \brief Copy-initializes an instance of this type.
		 * \param addr The address to initialize the instance at.
		 * \param copy The instance to copy-initialize from.
		 */
		virtual void copy_init(void* addr, const void* copy) const = 0;

		/**
		 * \brief Returns whether this type supports move-initialization.
		 */
		virtual bool has_move_init() const = 0;

		/**
		 * \brief Move-initializes an instance of this type.
		 * \param addr The address to initialize the instance at.
		 * \param move The instance to move-initialize from.
		 */
		virtual void move_init(void* addr, void* move) const = 0;

		/**
		 * \brief Returns whether this type supports copy-assignment.
		 */
		virtual bool has_copy_assign() const = 0;

		/**
		 * \brief Copy-assigns an instance of this type.
		 * \param self The instance to copy-assign to.
		 * \param copy The instance to copy-assign from.
		 */
		virtual void copy_assign(void* self, const void* copy) const = 0;

		/**
		 * \brief Returns whether this type supports move-assignment.
		 */
		virtual bool has_move_assign() const = 0;

		/**
		 * \brief Move-assigns an instance of this type.
		 * \param self The instance to move-assign to.
		 * \param move The instance to move-assign from.
		 */
		virtual void move_assign(void* self, void* move) const = 0;

		/**
		 * \brief Returns whether this type supports dropping (destruction).
		 */
		virtual bool has_drop() const = 0;

		/**
		 * \brief Drops (destroys) an instance of this type.
		 * \param self The instance to drop.
		 */
		virtual void drop(void* self) const = 0;

		/**
		 * \brief Searches for an unnamed constructor with the given number of arguments on this type.
		 * \param argc The arity of the constructor to search for.
		 * \return A pointer to the information for the constructor if found, 'nullptr' if not.
		 */
		virtual const ConstructorInfo* find_constructor(std::size_t argc) const = 0;

		/**
		 * \brief Enumerates all unnamed constructors on this type.
		 * \param enumerator A function to call with the arity and information for the constructor at each iteration.
		 */
		virtual void enumerate_constructors(FunctionView<ConstructorEnumeratorFn> enumerator) const = 0;

		/**
		 * \brief Searches for a named constructor with the given name on this type.
		 * \param name The name of the constructor to search for.
		 * \return A pointer to the information for the constructor if found, 'nullptr' otherwise.
		 */
		virtual const ConstructorInfo* find_named_constructor(const char* name) const = 0;

		/**
		 * \brief Enumerates all named constructors of this type.
		 * \param enumerator A functoin to call with the name and information for the constructor at each iteration.
		 */
		virtual void enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> enumerator) const = 0;

		/**
		 * \brief Searches for a property with the given name on this type.
		 * \param name The name of the property to search for.
		 * \return A pointer to the information for the property if found, 'nullptr' otherwise.
		 */
		virtual const PropertyInfo* find_property(const char* name) const = 0;

		/**
		 * \brief Enumerates all properties of this type.
		 * \param enumerator A function to call with the name and information for the property at each iteration.
		 */
		virtual void enumerate_properties(FunctionView<PropertyEnumeratorFn> enumerator) const = 0;

		/**
		 * \brief Searches for a field with the given name on this type.
		 * \param name The name of the field to search for.
		 * \return A pointer to the information of the field if found, 'nullptr' otherwise.
		 */
		virtual const FieldInfo* find_field(const char* name) const = 0;

		/**
		 * \brief Enumerates all fields of this type.
		 * \param enumerator A function to call with the name and information of the field at each iteration.
		 */
		virtual void enumerate_fields(FunctionView<FieldEnumeratorFn> enumerator) const = 0;

		/////////////////////
		///   Operators   ///
	public:

		friend bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
		{
			return &lhs == &rhs ||
				(lhs._data.native_type_info && rhs._data.native_type_info && *lhs._data.native_type_info == *rhs._data.native_type_info);
		}
		friend bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
		{
			return !(lhs == rhs);
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
