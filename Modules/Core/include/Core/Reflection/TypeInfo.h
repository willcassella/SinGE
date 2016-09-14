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

		/* Creates a property with a generic getter and generic setter. */
		template <typename GetT, typename SetT>
		TypeInfoBuilder&& property(
			std::string name,
			GetT getter,
			SetT setter,
			PropertyFlags flags = PF_NONE)
		{
			create_property(name, getter, setter, flags);
			return std::move(*this);
		}

		/* Creates a property with a method getter and generic setter. */
		template <typename GetRetT, typename SetT>
		TypeInfoBuilder&& property(
			std::string name, 
			GetRetT(T::*getter)()const,
			SetT setter,
			PropertyFlags flags = PF_NONE)
		{
			create_property(name, getter, setter, flags);
			return std::move(*this);
		}

		/* Creates a property with a method getter and setter. */
		template <typename GetRetT, typename SetRetT, typename SetArgT>
		TypeInfoBuilder&& property(
			std::string name,
			GetRetT(T::*getter)()const, 
			SetRetT(T::*setter)(SetArgT), 
			PropertyFlags flags = PF_NONE)
		{
			create_property(name, getter, setter, flags);
			return std::move(*this);
		}

	private:

		template <typename GetT, typename SetT>
		void create_property(std::string name, GetT getter, SetT setter, PropertyFlags flags)
		{
			using PropT = std::decay_t<typename stde::function_traits<GetT>::return_type>;

			auto prop = PropertyInfo::create<PropT>(name, flags);
			create_getter(prop, getter);
			create_setter<PropT>(prop, setter);
			result.properties.insert(std::make_pair(name, std::move(prop)));
		}

		/* Creates a getter for a method that does not require a context. */
		template <typename RetT>
		void create_getter(PropertyInfo& prop, RetT(T::*getter)() const)
		{
			prop.getter = [getter](const void* self, const void* /*context*/, PropertyInfo::GetterOut out) {
				const auto& result = (static_cast<const T*>(self)->*getter)();
				out(result);
			};
		}

		/* Creates a getter for a method that does require a context. */
		template <typename RetT, typename ContextT>
		void create_getter(PropertyInfo& prop, RetT(T::*getter)(ContextT) const)
		{
			prop.getter = [getter](const void* self, const void* context, PropertyInfo::GetterOut out) {
				const auto& result = (static_cast<const T*>(self)->*getter)(*static_cast<const std::decay_t<ContextT>*>(context));
				out(result);
			};
		}

		/* Creates a getter for a generic function that does not require a context. */
		template <typename GetT, typename GetF = stde::function_traits<GetT>>
		auto create_getter(PropertyInfo& prop, GetT getter) -> std::enable_if_t<GetF::arity == 1>
		{
			prop.getter = [getter](const void* self, const void* /*context*/, PropertyInfo::GetterOut out) {
				const auto& result = getter(static_cast<const T*>(self));
				out(result);
			};
		}

		/* Creates a getter for a generic function that does require a context. */
		template <typename GetT, typename GetF = stde::function_traits<GetT>>
		auto create_getter(PropertyInfo& prop, GetT getter) -> std::enable_if_t<GetF::arity == 2>
		{
			using ContextT = typename GetF::arg_types::at<1>;

			prop.getter = [getter](const void* self, const void* context, PropertyInfo::GetterOut out) {
				const auto& result = getter(static_cast<const T*>(self), static_cast<ContextT>(context));
				out(result);
			};
		}

		/* Does nothing. */
		template <typename PropT>
		void create_setter(PropertyInfo& /*prop*/, std::nullptr_t /*setter*/)
		{
		}

		/* Creates a setter for a method that does not require a context. */
		template <typename PropT, typename RetT, typename ArgT>
		void create_setter(PropertyInfo& prop, RetT(T::*setter)(ArgT))
		{
			using DecayedArgT = std::decay_t<ArgT>;
			static_assert(std::is_same<PropT, DecayedArgT>::value, "The getter and the setter use different property types.");
		
			prop.setter = [setter](void* self, const void* /*context*/, const void* value) {
				(static_cast<T*>(self)->*setter)(*static_cast<const DecayedArgT*>(value));
			};
		}

		/* Creates a setter for a method that does require a context. */
		template <typename PropT, typename RetT, typename ArgT, typename ContextT>
		void create_setter(PropertyInfo& prop, RetT(T::*setter)(ContextT, ArgT))
		{
			using DecayedContextT = std::decay_t<ContextT>;
			using DecayedArgT = std::decay_t<ArgT>;
			static_assert(std::is_same<PropT, DecayedArgT>::value, "The getter and the setter use different property types.");

			prop.setter = [setter](void* self, const void* context, const void* value) {
				(static_cast<T*>(self)->*setter)(*static_cast<const DecayedArgT*>(value), *static_cast<const DecayedContextT*>(context));
			};
		}

		/* Creates a setter for a generic function that does not require a context. */
		template <typename PropT, typename SetT, typename SetF = stde::function_traits<SetT>>
		auto create_setter(PropertyInfo& prop, SetT setter) -> std::enable_if_t<SetF::arity == 2>
		{
			using ArgT = typename SetF::arg_types::at<1>;
			static_assert(std::is_same<const PropT*, ArgT>::value, "The getter and the setter use different property types.");

			prop.setter = [setter](void* self, const void* /*context*/, const void* value) {
				setter(static_cast<T*>(self), *static_cast<ArgT>(value));
			};
		}

		/* Creates a setter for a generic function that does require a context. */
		template <typename PropT, typename SetT, typename SetF = stde::function_traits<SetT>>
		auto create_setter(PropertyInfo& prop, SetT setter) -> std::enable_if_t<SetF::arity == 3>
		{
			using ContextT = typename SetF::arg_types::at<1>;
			using ArgT = typename SetF::arg_types::at<2>;
			static_assert(std::is_same<const PropT*, ArgT>::value, "The getter and the setter use different property types.");

			prop.setter = [setter](void* self, const void* context, const void* value) {
				setter(static_cast<T*>(self), static_cast<ContextT>(context), static_cast<ArgT>(value));
			};
		}

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
