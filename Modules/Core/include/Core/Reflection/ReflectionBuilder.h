// ReflectionBuilder.h
#pragma once

#include "ArgAny.h"
#include "Reflection.h"

namespace sge
{
	template <typename T, typename ... ConvertedArgs>
	void constructor_wrapper(tmp::list<>, void* addr, const ArgAny* /*args*/, const ConvertedArgs&... realArgs)
	{
		new (addr) T(realArgs...);
	}

	template <typename T, typename ArgT, typename ... ArgTs, typename ... ConvertedArgs>
	void constructor_wrapper(tmp::list<ArgT, ArgTs...>, void* addr, const ArgAny* args, const ConvertedArgs&... realArgs)
	{
		constructor_wrapper<T>(
			tmp::list<ArgTs...>{},
			addr,
			args + 1,
			realArgs...,
			args->get<ArgT>());
	}

	template <typename T>
	struct NativeTypeInfoBuilder
	{
		////////////////////////
		///   Constructors   ///
	public:

		NativeTypeInfoBuilder(std::string name)
			: base_type_data(std::move(name))
		{
			if (std::is_empty<T>::value)
			{
				base_type_data.size = 0;
			}
			else
			{
				base_type_data.size = sizeof(T);
			}

			base_type_data.alignment = alignof(T&);
			base_type_data.native_type_info = &typeid(T);

			this->set_init<T>(0);
			this->set_copy_init<T>(0);
			this->set_move_init<T>(0);
			this->set_copy_assign<T>(0);
			this->set_move_assign<T>(0);
			this->set_drop<T>(0);
			this->set_equality_compare<T>(0);
		}

		///////////////////
		///   Methods   ///
	public:

		NativeTypeInfoBuilder&& flags(TypeFlags flags)
		{
			base_type_data.flags = flags;
			return std::move(*this);
		}

		template <typename BaseT>
		NativeTypeInfoBuilder&& extends()
		{
			static_assert(std::is_base_of<BaseT, T>::value, "The given type is not actually a base of this type.");
			base_type_data.base = &sge::get_type<BaseT>();
			return std::move(*this);
		}

		template <typename InterfaceT>
		NativeTypeInfoBuilder&& implements()
		{
			type_data.interfaces.insert(std::make_pair(&sge::get_interface<InterfaceT>(), &InterfaceT::template get_impl<T>()));
			return std::move(*this);
		}

		template <typename ArgT, typename ... ArgTs>
		NativeTypeInfoBuilder&& constructor()
		{
			ConstructorInfo::Data baseCData;
			baseCData.arg_types = { &sge::get_type<ArgT>(), &sge::get_type<ArgTs>()... };

			NativeConstructorInfo::Data cData;
			cData.constructor = [](void* addr, const ArgAny* args) -> void {
				sge::constructor_wrapper<T>(tmp::list<ArgT, ArgTs...>{}, addr, args);
			};

			type_data.constructors.insert(std::make_pair(sizeof...(ArgTs)+1, NativeConstructorInfo{ std::move(baseCData), std::move(cData) }));
			return std::move(*this);
		}

		template <typename ArgT, typename ... ArgTs>
		NativeTypeInfoBuilder&& constructor(T(*constructor)(ArgTs...))
		{
			ConstructorInfo::Data baseCData;
			baseCData.arg_types = { &sge::get_type<ArgT>(), &sge::get_type<ArgTs>()... };

			NativeConstructorInfo::Data cData;
			cData.constructor = [constructor](void* addr, const ArgAny* args) -> void {
				sge::constructor_wrapper<T>(tmp::list<ArgT, ArgTs...>{}, addr, args);
			};

			type_data.constructors.insert(std::make_pair(sizeof...(ArgTs)+1, NativeConstructorInfo{ std::move(baseCData), std::move(cData) }));
			return std::move(*this);
		}

		template <typename ... ArgTs>
		NativeTypeInfoBuilder&& named_constructor(const char* name)
		{
			ConstructorInfo::Data baseCData;
			baseCData.arg_types = { &sge::get_type<ArgTs>()... };

			NativeConstructorInfo::Data cData;
			cData.constructor = [](void* addr, const ArgAny* args) -> void {
				sge::constructor_wrapper<T>(tmp::list<ArgTs...>{}, addr, args);
			};

			type_data.named_constructors.insert(std::make_pair(name, NativeConstructorInfo{ std::move(baseCData), std::move(cData) }));
			return std::move(*this);
		}

		template <typename ... ArgTs>
		NativeTypeInfoBuilder&& named_constructor(const char* name, T(*constructor)(ArgTs...))
		{
			ConstructorInfo::Data baseCData;
			baseCData.arg_types = { &sge::get_type<ArgTs>()... };

			NativeConstructorInfo::Data cData;
			cData.constructor = [constructor](void* addr, const ArgAny* args) -> void {
				sge::constructor_wrapper<T>(tmp::list<ArgTs...>{}, addr, args);
			};

			type_data.named_constructors.insert(std::make_pair(name, NativeConstructorInfo{ std::move(baseCData), std::move(cData) }));
			return std::move(*this);
		}

		/* Creates a property with a generic getter and generic setter. */
		template <typename GetFn, typename SetFn>
		NativeTypeInfoBuilder&& property(
			const char* name,
			GetFn getter,
			SetFn setter,
			PropertyFlags flags = PF_NONE)
		{
			using GetFnTraits = stde::function_traits<GetFn>;
			using SetFnTraits = stde::function_traits<SetFn>;
			using PropT = std::decay_t<typename GetFnTraits::return_type>;

			static_assert(std::is_same<const PropT*, tmp::car_n<typename SetFnTraits::arg_types, 1>>::value,
				"Property type differs between getter and setter");

			auto adaptedGetter = adapt_function_getter(getter);
			auto adaptedSetter = adapt_function_setter(setter);
			create_property<PropT>(name, adaptedGetter, adaptedSetter, flags);
			return std::move(*this);
		}

		/* Creates a property with a function getter and no setter. */
		template <typename GetFn>
		NativeTypeInfoBuilder&& property(
			const char* name,
			GetFn getter,
			std::nullptr_t /*setter*/,
			PropertyFlags flags = PF_NONE)
		{
			using GetFnTraits = stde::function_traits<GetFn>;
			using PropT = std::decay_t<typename GetFnTraits::return_type>;

			auto adaptedGetter = adapt_function_getter(getter);
			create_readonly_property<PropT>(name, adaptedGetter, flags);
			return std::move(*this);
		}

		/* Creates a property with a method getter and setter. */
		template <typename GetRetT, typename SetRetT, typename SetArgT>
		NativeTypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)()const,
			SetRetT(T::*setter)(SetArgT),
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			static_assert(std::is_same<PropT, std::decay_t<SetArgT>>::value, "Getter and setter use different property types.");

			auto adaptedGetter = adapt_method_getter(getter);
			auto adaptedSetter = adapt_method_setter(setter);
			create_property<PropT>(name, adaptedGetter, adaptedSetter, flags);
			return std::move(*this);
		}

		/* Creates a property with a method getter and no setter. */
		template <typename GetRetT>
		NativeTypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)()const,
			std::nullptr_t /*setter*/,
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;

			auto adaptedGetter = adapt_method_getter(getter);
			create_readonly_property<PropT>(name, adaptedGetter, flags);
			return std::move(*this);
		}

		/* Registers a field. */
		template <typename FieldT>
		NativeTypeInfoBuilder&& field(
			const char* name,
			FieldT T::*field,
			FieldFlags flags = FF_NONE)
		{
			create_field(name, field, flags);
			return std::move(*this);
		}

		/* Registers a field, and a corresponding property. */
		template <typename FieldT>
		NativeTypeInfoBuilder&& field_property(
			const char* name,
			FieldT T::*field,
			FieldFlags fieldFlags = FF_NONE,
			PropertyFlags propertyFlags = PF_NONE)
		{
			create_field(name, field, fieldFlags);

			PropertyInfo::Data basePropData;
			basePropData.type = &sge::get_type<FieldT>();
			basePropData.flags = propertyFlags;

			NativePropertyInfo::Data propData;
			propData.getter = create_field_getter(field);
			propData.setter = create_field_setter(field);
			propData.mutate = create_field_mutate(field);

			type_data.properties.insert(std::make_pair(name, NativePropertyInfo{ std::move(basePropData), std::move(propData) }));

			return std::move(*this);
		}

		/* Registers a field, and a corresponding readonly property. */
		template <typename FieldT>
		NativeTypeInfoBuilder&& field_readonly_property(
			const char* name,
			FieldT T::*field,
			FieldFlags fieldFlags = FF_NONE,
			PropertyFlags propertyFlags = PF_NONE)
		{
			create_field(name, field, fieldFlags);

			PropertyInfo::Data basePropData;
			basePropData.type = &sge::get_type<FieldT>();
			basePropData.flags = propertyFlags;

			NativePropertyInfo::Data propData;
			create_field_getter(propData, field);

			type_data.properties.insert(std::make_pair(name, NativePropertyInfo{ std::move(basePropData), std::move(propData) }));

			return std::move(*this);
		}

	private:

		template <typename FieldT>
		static std::size_t get_field_offset(FieldT T::*field)
		{
			// Bit of a hack, but necessary. If this becomes problematic, I can replace the field offset with a getter/setter std::function pair or something.
			// Though that would be much less performant.
			alignas(T&) const char fake[sizeof(T)] = {};
			const char* pField = reinterpret_cast<const char*>(&(reinterpret_cast<const T*>(&fake)->*field));

			return pField - &fake[0];
		}

		template <typename PropT, typename GetFn, typename SetFn>
		void create_property(const char* name, GetFn getter, SetFn setter, PropertyFlags flags)
		{
			PropertyInfo::Data basePropData;
			basePropData.type = &sge::get_type<PropT>();
			basePropData.flags = flags;

			NativePropertyInfo::Data propData;
			propData.getter = create_getter<PropT>(getter);
			propData.setter = create_setter<PropT>(setter);
			propData.mutate = create_mutate<PropT>(getter, setter);

			type_data.properties.insert(std::make_pair(name, NativePropertyInfo{ std::move(basePropData), std::move(propData) }));
		}

		template <typename PropT, typename GetFn>
		void create_readonly_property(const char* name, GetFn getter, PropertyFlags flags)
		{
			PropertyInfo::Data basePropData;
			basePropData.type = &sge::get_type<PropT>();
			basePropData.flags = flags;

			NativePropertyInfo::Data propData;
			propData.getter = create_getter<PropT>(getter);

			type_data.properties.insert(std::make_pair(name, NativePropertyInfo{ std::move(basePropData), std::move(propData) }));
		}

		template <typename FieldT>
		void create_field(const char* name, FieldT T::*field, FieldFlags flags)
		{
			FieldInfo::Data fieldData;
			fieldData.flags = flags;
			fieldData.type = &sge::get_type<FieldT>();
			fieldData.offset = get_field_offset(field);

			type_data.fields.insert(std::make_pair(name, std::move(fieldData)));
		}

		template <typename GetFn>
		static auto adapt_function_getter(GetFn getter)
		{
			using FnTraits = stde::function_traits<GetFn>;
			using RetT = typename FnTraits::return_type;

			return [getter](const T* self) -> RetT {
				return getter(self);
			};
		}

		template <typename RetT>
		static auto adapt_method_getter(RetT(T::*getter)()const)
		{
			return [getter](const T* self) -> RetT {
				return (self->*getter)();
			};
		}

		template <typename SetFn>
		static auto adapt_function_setter(SetFn setter)
		{
			return [setter](T* self, const auto& value) -> void {
				setter(self, value);
			};
		}

		template <typename RetT, typename PropArgT>
		static auto adapt_method_setter(RetT(T::*setter)(PropArgT))
		{
			return [setter](T* self, const auto& value) -> void {
				(self->*setter)(value);
			};
		}

		template <typename PropT, typename GetFn>
		static auto create_getter(GetFn getter)
		{
			return [getter](const void* self, PropertyInfo::GetterOutFn out) -> void {
				const PropT& value = getter(static_cast<const T*>(self));
				out(value);
			};
		}

		template <typename PropT, typename SetFn>
		static auto create_setter(SetFn setter)
		{
			return [setter](void* self, const void* value) -> void {
				setter(static_cast<T*>(self), *static_cast<const PropT*>(value));
			};
		}

		template <typename PropT, typename GetFn, typename SetFn>
		static auto create_mutate(GetFn getter, SetFn setter)
		{
			return [getter, setter](void* self, PropertyInfo::MutatorFn mutator) -> void {
				PropT prop = getter(static_cast<const T*>(self));
				mutator(prop);
				setter(static_cast<T*>(self), prop);
			};
		}

		template <typename FieldT>
		static auto create_field_getter(FieldT T::*field)
		{
			return [field](const void* self, PropertyInfo::GetterOutFn out) -> void {
				out(static_cast<const T*>(self)->*field);
			};
		}

		template <typename FieldT>
		static auto create_field_setter(FieldT T::*field)
		{
			return [field](void* self, const void* value) -> void {
				static_cast<T*>(self)->*field = *static_cast<const FieldT*>(value);
			};
		}

		template <typename FieldT>
		static auto create_field_mutate(FieldT T::*field)
		{
			return [field](void* self, PropertyInfo::MutatorFn mutate) -> void {
				mutate(static_cast<T*>(self)->*field);
			};
		}

		template <typename F>
		auto set_init(int) -> std::enable_if_t<std::is_default_constructible<F>::value>
		{
			type_data.init = [](void* addr) -> void {
				new(addr) F();
			};
		}

		template <typename F>
		void set_init(char)
		{
		}

		template <typename F>
		auto set_copy_init(int) -> std::enable_if_t<std::is_copy_constructible<F>::value>
		{
			type_data.copy_init = [](void* addr, const void* copy) -> void {
				new(addr) F(*static_cast<const F*>(copy));
			};
		}

		template <typename F>
		void set_copy_init(char)
		{
		}

		template <typename F>
		auto set_move_init(int) -> std::enable_if_t<std::is_move_constructible<F>::value>
		{
			type_data.move_init = [](void* addr, void* move) -> void {
				new(addr) T(std::move(*static_cast<T*>(move)));
			};
		}

		template <typename F>
		void set_move_init(char)
		{
		}

		template <typename F>
		auto set_copy_assign(int) -> std::enable_if_t<std::is_copy_assignable<F>::value>
		{
			type_data.copy_assign = [](void* self, const void* copy) -> void {
				*static_cast<T*>(self) = *static_cast<const T*>(copy);
			};
		}

		template <typename F>
		void set_copy_assign(char)
		{
		}

		template <typename F>
		auto set_move_assign(int) -> std::enable_if_t<std::is_move_assignable<F>::value>
		{
			type_data.move_assign = [](void* self, void* move) -> void {
				*static_cast<T*>(self) = std::move(*static_cast<T*>(move));
			};
		}

		template <typename F>
		void set_move_assign(char)
		{
		}

		template <typename F>
		auto set_drop(int) -> std::enable_if_t<std::is_destructible<F>::value>
		{
			type_data.drop = [](void* self) -> void {
				static_cast<T*>(self)->~T();
			};
		}

		template <typename F>
		void set_drop(char)
		{
		}

		template <typename F>
		auto set_equality_compare(int) -> std::enable_if_t<std::is_same<decltype(std::declval<const F&>() == std::declval<const F&>()), bool>::value>
		{
			type_data.equality_compare = [](const void* lhs, const void* rhs) -> bool {
				return *static_cast<const F*>(lhs) == *static_cast<const F*>(rhs);
			};
		}

		template <typename F>
		void set_equality_compare(char)
		{
		}

		//////////////////
		///   Fields   ///
	public:

		TypeInfo::Data base_type_data;
		NativeTypeInfo::Data type_data;
	};

	template <class I>
	struct InterfaceInfoBuilder final
	{
		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfoBuilder(std::string name)
		{
			interface_data.name = std::move(name);
		}

		//////////////////
		///   Fields   ///
	public:

		InterfaceInfo::Data interface_data;

		///////////////////
		///   Methods   ///
	public:

		template <typename T>
		InterfaceInfoBuilder&& implemented_for()
		{
			interface_data.implementations.insert(std::make_pair(&sge::get_type<T>(), &I::template get_impl<T>()));
			return std::move(*this);
		}
	};
}

//////////////////
///   Macros   ///

/* Use this macro in the source file for a type to define its reflection data. */
#define SGE_REFLECT_TYPE(TYPE)		const ::sge::NativeTypeInfo TYPE::type_info = ::sge::NativeTypeInfoBuilder<TYPE>(#TYPE)

/* Use this macro in the source file for an interface to define its reflection data. */
#define SGE_REFLECT_INTERFACE(INTERF)	const ::sge::InterfaceInfo INTERF::interface_info = ::sge::InterfaceInfoBuilder<INTERF>(#INTERF)

/* Use this macro in the header file for a template type to define its reflection datas. */
#define SGE_REFLECT_TYPE_TEMPLATE(TYPE, ...)							\
	namespace sge { namespace specialized {								\
	template <__VA_ARGS__> struct GetType < TYPE<__VA_ARGS__> >{		\
		static const ::sge::NativeTypeInfo type_info;					\
		static const auto& get_type() {									\
			return type_info;											\
		}																\
		static const auto& get_type(const TYPE<__VA_ARGS__>&) {			\
			return type_info;											\
		}																\
	};																	\
	template <__VA_ARGS__>												\
	const ::sge::NativeTypeInfo GetType<TYPE<__VA_ARGS__>>::type_info =	\
		NativeTypeInfoBuilder<TYPE<__VA_ARGS__>>(#TYPE); }}
