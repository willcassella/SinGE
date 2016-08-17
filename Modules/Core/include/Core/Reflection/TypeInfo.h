// TypeInfo.h
#pragma once

#include <map>
#include "../config.h"

namespace singe
{
	////////////////////////////////
	///   Forward Declarations   ///

	/* Defined in 'InterfaceInfo.h' */
	struct InterfaceInfo;

	/* Defined below */
	template <typename T>
	struct TypeInfoBuilder;
	
	/* Defined in 'Reflection.h' */
	template <typename T>
	const InterfaceInfo& get_interface();

	/////////////////
	///   Types   ///

	struct CORE_API TypeInfo final
	{
		////////////////////////
		///   Constructors   ///
	public:

		TypeInfo();

		template <typename T>
		TypeInfo(TypeInfoBuilder<T>&& builder)
			: TypeInfo{ std::move(builder.result) }
		{
		}

		//////////////////
		///   Fields   ///
	public:

		std::string name;

		/* The size (in bytes) of an instance of this type. */
		size_t size;

		/* The alignment requirements of an instance of this type. */
		size_t alignment;

		/* Default-initializes a new instance of this type. */
		void(C_CALL*initialize)(void*);

		/* Copy-initializes a new instance of this type. */
		void(C_CALL*copy_initialize)(void*, const void*);

		/* Move-initializes a new instance of this type. */
		void(C_CALL*move_initialize)(void*, void*);

		/* Copy-assigns one instance of this type to another. */
		void(C_CALL*copy_assign)(void*, const void*);

		/* Move-assigns one instance of this type to another. */
		void(C_CALL*move_assign)(void*, void*);

		/* Deinitializes an instance of this type. */
		void(C_CALL*drop)(void*);

		/* Base class for instances of this type. */
		const TypeInfo* base;

		/* Map Associating interface objects to their implementations. */
		std::map<const InterfaceInfo*, const void*> interfaces;
	};

	template <typename T>
	struct TypeInfoBuilder final
	{
		////////////////////////
		///   Constructors   ///
	public:

		TypeInfoBuilder(std::string name)
		{
			if (std::is_empty<T>::value)
			{
				result.size = 0;
			}
			else
			{
				result.size = sizeof(T);
			}

			result.name = std::move(name);
			result.alignment = alignof(T&);

			this->set_initialize<T>();
			this->set_copy_initialize<T>();
			this->set_move_initialize<T>();
			this->set_copy_assign<T>();
			this->set_move_assign<T>();
			this->set_drop<T>();
		}

		//////////////////
		///   Fields   ///
	public:

		TypeInfo result;

		///////////////////
		///   Methods   ///
	public:

		template <typename InterfaceT>
		TypeInfoBuilder&& auto_impl()
		{
			static InterfaceT impl = InterfaceT::auto_impl<T>();
			result.interfaces[&get_interface<InterfaceT>()] = &impl;
			return std::move(*this);
		}

		template <typename InterfaceT>
		TypeInfoBuilder&& custom_impl(InterfaceT customImpl)
		{
			static InterfaceT impl = std::move(customImpl);
			result.interfaces[&get_interface<InterfaceT>()] = &impl;
			return std::move(*this);
		}

		template <typename BaseT>
		TypeInfoBuilder&& extends()
		{
			static_assert(std::is_base_of<BaseT, T>::value, "The given type is not actually a base of this type.");
			result.base = &get_type<BaseT>();
			return std::move(*this);
		}

	private:

		template <typename F>
		auto set_initialize() -> std::enable_if_t<std::is_default_constructible<F>::value>
		{
			result.initialize = [](void* addr) {
				new(addr) F{};
			};
		}

		template <typename F>
		auto set_initialize() -> std::enable_if_t<!std::is_default_constructible<F>::value>
		{
			result.initialize = nullptr;
		}

		template <typename F>
		auto set_copy_initialize() -> std::enable_if_t<std::is_copy_constructible<F>::value>
		{
			result.copy_initialize = [](void* addr, const void* copy) {
				new(addr) F{ *static_cast<const F*>(copy) };
			};
		}

		template <typename F>
		auto set_copy_initialize() -> std::enable_if_t<!std::is_copy_constructible<F>::value>
		{
			result.copy_initialize = nullptr;
		}

		template <typename F>
		auto set_move_initialize() -> std::enable_if_t<std::is_move_constructible<F>::value>
		{
			result.move_initialize = [](void* addr, void* move) {
				new(addr) T{ std::move(*static_cast<T*>(move)) };
			};
		}

		template <typename F>
		auto set_move_initialize() -> std::enable_if_t<!std::is_move_constructible<F>::value>
		{
			result.move_initialize = nullptr;
		}

		template <typename F>
		auto set_copy_assign() -> std::enable_if_t<std::is_copy_assignable<F>::value>
		{
			result.copy_assign = [](void* self, const void* copy) {
				*static_cast<T*>(self) = *static_cast<const T*>(copy);
			};
		}

		template <typename F>
		auto set_copy_assign() -> std::enable_if_t<!std::is_copy_assignable<F>::value>
		{
			result.copy_assign = nullptr;
		}

		template <typename F>
		auto set_move_assign() -> std::enable_if_t<std::is_move_assignable<F>::value>
		{
			result.move_assign = [](void* self, void* move) {
				*static_cast<T*>(self) = std::move(*static_cast<T*>(move));
			};
		}

		template <typename F>
		auto set_move_assign() -> std::enable_if_t<!std::is_move_assignable<F>::value>
		{
			result.move_assign = nullptr;
		}

		template <typename F>
		auto set_drop() -> std::enable_if_t<std::is_destructible<F>::value>
		{
			result.drop = [](void* self) {
				static_cast<T*>(self)->~T();
			};
		}

		template <typename F>
		auto set_drop() -> std::enable_if_t<!std::is_destructible<F>::value>
		{
			result.drop = nullptr;
		}
	};
}
