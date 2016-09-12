// TypeInfo.h
#pragma once

#include <map>
#include <unordered_map>
#include "../config.h"
#include "PropertyInfo.h"

namespace sge
{
	struct TypeInfo;

	struct InterfaceInfo;
	
	struct PropertyInfo;

	template <typename T>
	struct TypeInfoBuilder;

	template <typename T>
	const TypeInfo& get_type();

	template <class I>
	const InterfaceInfo& get_interface();

	struct SGE_CORE_API TypeInfo
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
		void(*init)(void*);

		/* Copy-initializes a new instance of this type. */
		void(*copy_init)(void*, const void*);

		/* Move-initializes a new instance of this type. */
		void(*move_init)(void*, void*);

		/* Copy-assigns one instance of this type to another. */
		void(*copy_assign)(void*, const void*);

		/* Move-assigns one instance of this type to another. */
		void(*move_assign)(void*, void*);

		/* Deinitializes an instance of this type. */
		void(*drop)(void*);

		/* Base class for instances of this type. */
		const TypeInfo* base;

		/* Map Associating interface objects to their implementations. */
		std::unordered_map<const InterfaceInfo*, const void*> interfaces;

		/* Map associating property names to property objects. */
		std::map<std::string, PropertyInfo> properties;
	};

	template <typename T>
	struct TypeInfoBuilder
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

			this->set_init<T>();
			this->set_copy_init<T>();
			this->set_move_init<T>();
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

		template <typename BaseT>
		TypeInfoBuilder&& extends()
		{
			static_assert(std::is_base_of<BaseT, T>::value, "The given type is not actually a base of this type.");
			result.base = &get_type<BaseT>();
			return std::move(*this);
		}

		template <typename InterfaceT>
		TypeInfoBuilder&& implements()
		{
			static InterfaceT vtable = InterfaceT::template get_vtable<T>();
			result.interfaces[&get_interface<InterfaceT>()] = &vtable;
			return std::move(*this);
		}

		/* Creates a property with a method getter and no setter. */
		template <typename PropT>
		TypeInfoBuilder&& property(
			std::string name, 
			PropT(T::*getter)()const, 
			std::nullptr_t /*setter*/, 
			PropertyFlags flags = PF_NONE)
		{
			auto prop = PropertyInfo::create<PropT>(name, flags);
			prop.getter = [getter](const void* self, const void* context, PropertyInfo::GetterOut out) {
				const auto& result = (static_cast<const T*>(self)->*getter)();
				out(result);
			};

			result.properties.insert(std::make_pair(name, std::move(prop)));
			return std::move(*this);
		}

		/* Creates a property with a method getter and setter. */
		template <typename PropT>
		TypeInfoBuilder&& property(
			std::string name,
			PropT(T::*getter)()const, 
			void(T::*setter)(PropT), 
			PropertyFlags flags = PF_NONE)
		{
			auto prop = PropertyInfo::create<PropT>(name, flags);
			prop.getter = [getter](const void* self, const void* context, PropertyInfo::GetterOut out) {
				const auto& result = (static_cast<const T*>(self)->*getter)();
				out(result);
			};
			prop.setter = [setter](void* self, const void* context, const void* value) {
				(static_cast<T*>(self)->*setter)(*static_cast<const PropT*>(value));
			};
			
			result.properties.insert(std::make_pair(name, std::move(prop)));
			return std::move(*this);
		}

	private:

		template <typename F>
		auto set_init() -> std::enable_if_t<std::is_default_constructible<F>::value>
		{
			result.init = [](void* addr) {
				new(addr) F{};
			};
		}

		template <typename F>
		auto set_init() -> std::enable_if_t<!std::is_default_constructible<F>::value>
		{
			result.init = nullptr;
		}

		template <typename F>
		auto set_copy_init() -> std::enable_if_t<std::is_copy_constructible<F>::value>
		{
			result.copy_init = [](void* addr, const void* copy) {
				new(addr) F{ *static_cast<const F*>(copy) };
			};
		}

		template <typename F>
		auto set_copy_init() -> std::enable_if_t<!std::is_copy_constructible<F>::value>
		{
			result.copy_init = nullptr;
		}

		template <typename F>
		auto set_move_init() -> std::enable_if_t<std::is_move_constructible<F>::value>
		{
			result.move_init = [](void* addr, void* move) {
				new(addr) T{ std::move(*static_cast<T*>(move)) };
			};
		}

		template <typename F>
		auto set_move_init() -> std::enable_if_t<!std::is_move_constructible<F>::value>
		{
			result.move_init = nullptr;
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
