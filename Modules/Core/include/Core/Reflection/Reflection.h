// Reflection.h
#pragma once

#include <map>
#include "../STDE/TypeTraits.h"
#include "Any.h"
#include "TypeInfo.h"
#include "InterfaceInfo.h"

namespace sge
{
	namespace specialized
	{
		/* Generic implementation of 'GetType' */
		template <typename T>
		struct GetType
		{
			static const TypeInfo& get_type()
			{
				return T::type_info;
			}

			static const TypeInfo& get_type(const T& value)
			{
				return value.get_type();
			}
		};

		template <>
		struct SGE_CORE_API GetType< void >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType< bool >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < char >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < byte >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int16 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint16 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int32 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint32 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int64 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint64 >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < float >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < double > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < long double >
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};
	}

	template <class I, typename T>
	struct Impl;

	/////////////////////
	///   Functions   ///

	template <typename I>
	const InterfaceInfo& get_interface()
	{
		return I::interface_info;
	}

	template <typename T>
	const TypeInfo& get_type()
	{
		return specialized::GetType<T>::get_type();
	}

	template <typename T>
	const TypeInfo& get_type(const T& value)
	{
		return specialized::GetType<T>::get_type(value);
	}

	template <typename T>
	struct TypeInfoBuilder
	{
		////////////////////////
		///   Constructors   ///
	public:

		TypeInfoBuilder(std::string name)
			: result{ std::move(name) }
		{
			if (std::is_empty<T>::value)
			{
				result.size = 0;
			}
			else
			{
				result.size = sizeof(T);
			}

			result.alignment = alignof(T&);

			this->set_init<T>();
			this->set_copy_init<T>();
			this->set_move_init<T>();
			this->set_copy_assign<T>();
			this->set_move_assign<T>();
			this->set_drop<T>();
		}

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
		template <typename GetFn, typename SetFn>
		TypeInfoBuilder&& property(
			const char* name,
			GetFn getter,
			SetFn setter,
			PropertyFlags flags = PF_NONE)
		{
			using GetFnTraits = stde::function_traits<GetFn>;
			using SetFnTraits = stde::function_traits<SetFn>;
			using PropT = std::decay_t<typename GetFnTraits::return_type>;
			using ContextT = std::remove_pointer_t<typename SetFnTraits::arg_types::template at<1>>;

			static_assert(std::is_same<const PropT*, typename SetFnTraits::arg_types::template at<2>>::value,
				"Property type differs between getter and setter");
			static_assert(std::is_same<const ContextT*, typename GetFnTraits::arg_types::template at<1>>::value,
				"Context type differs between getter and setter.");

			auto adaptedGetter = adapt_function_getter(getter);
			auto adaptedSetter = adapt_function_setter(setter);
			create_property<PropT>(name, adaptedGetter, adaptedSetter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Creates a property with a function getter and no setter. */
		template <typename GetFn>
		TypeInfoBuilder&& property(
			const char* name,
			GetFn getter,
			std::nullptr_t /*setter*/,
			PropertyFlags flags = PF_NONE)
		{
			using GetFnTraits = stde::function_traits<GetFn>;
			using PropT = std::decay_t<typename GetFnTraits::return_type>;
			using ContextT = std::remove_const_t<std::remove_pointer_t<typename GetFnTraits::arg_types::template at<1>>>;

			auto adaptedGetter = adapt_function_getter(getter);
			create_readonly_property<PropT>(name, adaptedGetter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Creates a property with a method getter and setter. */
		template <typename GetRetT, typename SetRetT, typename SetArgT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)()const,
			SetRetT(T::*setter)(SetArgT),
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			static_assert(std::is_same<PropT, std::decay_t<SetArgT>>::value, "Getter and setter use different property types.");

			auto adaptedGetter = adapt_method_getter(getter);
			auto adaptedSetter = adapt_method_setter(setter);
			create_property<PropT>(name, adaptedGetter, adaptedSetter, flags, nullptr);
			return std::move(*this);
		}

		/* Creates a property with a method getter and no setter. */
		template <typename GetRetT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)()const,
			std::nullptr_t /*setter*/,
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;

			auto adaptedGetter = adapt_method_getter(getter);
			create_readonly_property<PropT>(name, adaptedGetter, flags, nullptr);
			return std::move(*this);
		}

		/* Creates a property with a method getter and setter, where the getter requires a context. */
		template <typename GetRetT, typename GetContextT, typename SetRetT, typename SetArgT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)(GetContextT)const,
			SetRetT(T::*setter)(SetArgT),
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			using ContextT = std::decay_t<GetContextT>;
			static_assert(std::is_same<PropT, std::decay_t<SetArgT>>::value, "Getter and setter use different property types.");

			auto adaptedGetter = adapt_method_getter(getter);
			auto adaptedSetter = adapt_method_setter(setter);
			create_property<PropT>(name, getter, setter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Creates a property with a method getter and no setter, where the getter requires a context. */
		template <typename GetRetT, typename GetContextT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)(GetContextT)const,
			std::nullptr_t /*setter*/,
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			using ContextT = std::decay_t<GetContextT>;

			auto adaptedGetter = adapt_method_getter(getter);
			create_readonly_property<PropT>(name, getter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Creates a property with a method getter and method setter, where the setter requires a context. */
		template <typename GetRetT, typename SetRetT, typename SetContextT, typename SetArgT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)()const,
			SetRetT(T::*setter)(SetContextT, SetArgT),
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			using ContextT = std::decay_t<SetContextT>;
			static_assert(std::is_same<PropT, std::decay_t<SetArgT>>::value, "Getter and setter use different property types.");

			auto adaptedGetter = adapt_method_getter(getter);
			auto adaptedSetter = adapt_method_setter(setter);
			create_property<PropT>(name, adaptedGetter, adaptedSetter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Creates a property with a method getter and method setter, where both require a context. */
		template <typename GetRetT, typename GetContextT, typename SetRetT, typename SetContextT, typename SetArgT>
		TypeInfoBuilder&& property(
			const char* name,
			GetRetT(T::*getter)(GetContextT)const,
			SetRetT(T::*setter)(SetContextT, SetArgT),
			PropertyFlags flags = PF_NONE)
		{
			using PropT = std::decay_t<GetRetT>;
			using ContextT = std::decay_t<GetContextT>;
			static_assert(std::is_same<PropT, std::decay_t<SetArgT>>::value, "Getter and setter use different property types.");
			static_assert(std::is_same<ContextT, std::decay_t<SetContextT>>::value, "Getter and setter use different context types.");

			auto adaptedGetter = adapt_method_getter(getter);
			auto adaptedSetter = adapt_method_setter(setter);
			create_property<PropT>(name, getter, setter, flags, &get_type<ContextT>());
			return std::move(*this);
		}

		/* Registers a field. */
		template <typename FieldT>
		TypeInfoBuilder&& field(
			const char* name,
			FieldT T::*field,
			FieldFlags flags = FF_NONE)
		{
			create_field(name, field, flags);
			return std::move(*this);
		}

		/* Registers a field, and a corresponding property. */
		template <typename FieldT>
		TypeInfoBuilder&& field_property(
			const char* name,
			FieldT T::*field,
			FieldFlags fieldFlags = FF_NONE,
			PropertyFlags propertyFlags = PF_NONE)
		{
			create_field(name, field, fieldFlags);

			auto propData = PropertyInfo::Data{ &get_type<FieldT>(), nullptr, propertyFlags };
			create_field_getter(propData, field);
			create_field_setter(propData, field);
			create_field_mutate(propData, field);
			result.properties.insert(std::make_pair(name, std::move(propData)));

			return std::move(*this);
		}

		/* Registers a field, and a corresponding readonly property. */
		template <typename FieldT>
		TypeInfoBuilder&& field_readonly_property(
			const char* name,
			FieldT T::*field,
			FieldFlags fieldFlags = FF_NONE,
			PropertyFlags propertyFlags = PF_NONE)
		{
			create_field(name, field, fieldFlags);

			auto propData = PropertyInfo::Data{ &get_type<FieldT>(), nullptr, propertyFlags };
			create_field_getter(propData, field);
			result.properties.insert(std::make_pair(name, std::move(propData)));

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
		void create_property(const char* name, GetFn getter, SetFn setter, PropertyFlags flags, const TypeInfo* contextType)
		{
			auto propData = PropertyInfo::Data{ &get_type<PropT>(), contextType, flags };
			propData.getter = create_getter<PropT>(getter);
			propData.setter = create_setter<PropT>(setter);
			propData.mutate = create_mutate<PropT>(getter, setter);

			result.properties.insert(std::make_pair(name, std::move(propData)));
		}

		template <typename PropT, typename GetFn>
		void create_readonly_property(const char* name, GetFn getter, PropertyFlags flags, const TypeInfo* contextType)
		{
			auto propData = PropertyInfo::Data{ &get_type<PropT>(), contextType, flags };
			propData.getter = create_getter<PropT>(getter);

			result.properties.insert(std::make_pair(name, std::move(propData)));
		}

		template <typename FieldT>
		void create_field(const char* name, FieldT T::*field, FieldFlags flags)
		{
			auto info = FieldInfo{ &get_type<FieldT>(), flags, get_field_offset(field) };

			result.fields.insert(std::make_pair(name, std::move(info)));
		}

		template <typename GetFn>
		static auto adapt_function_getter(GetFn getter)
		{
			using FnTraits = stde::function_traits<GetFn>;
			using RetT = typename FnTraits::return_type;
			using PContextT = typename FnTraits::arg_types::template at<1>;

			return [getter](const T* self, const void* context) -> RetT {
				return getter(self, static_cast<PContextT>(context));
			};
		}

		template <typename RetT>
		static auto adapt_method_getter(RetT(T::*getter)()const)
		{
			return [getter](const T* self, const void* /*context*/) -> RetT {
				return (self->*getter)();
			};
		}

		template <typename RetT, typename ContextArgT>
		static auto adapt_method_getter(RetT(T::*getter)(ContextArgT)const)
		{
			using ContextT = std::decay_t<ContextArgT>;

			return [getter](const T* self, const void* context) -> RetT {
				return (self->*getter)(*static_cast<const ContextT*>(context));
			};
		}

		template <typename SetFn>
		static auto adapt_function_setter(SetFn setter)
		{
			using FnTraits = stde::function_traits<SetFn>;
			using PContextT = typename FnTraits::arg_types::template at<1>;
			using PPropT = typename FnTraits::arg_types::template at<2>;

			return [setter](T* self, void* context, PPropT value) -> void {
				setter(self, static_cast<PContextT>(context), value);
			};
		}

		template <typename RetT, typename PropArgT>
		static auto adapt_method_setter(RetT(T::*setter)(PropArgT))
		{
			using PropT = std::decay_t<PropArgT>;

			return [setter](T* self, void* /*context*/, const PropT* value) -> void {
				(self->*setter)(*value);
			};
		}

		template <typename RetT, typename ContextArgT, typename PropArgT>
		static auto adapt_method_setter(RetT(T::*setter)(ContextArgT, PropArgT))
		{
			using ContextT = std::decay_t<ContextArgT>;
			using PropT = std::decay_t<PropArgT>;

			return [setter](T* self, void* context, const PropT* value) -> void {
				(self->*setter)(*static_cast<ContextT*>(context), *value);
			};
		}

		template <typename PropT, typename GetFn>
		static auto create_getter(GetFn getter)
		{
			return [getter](const void* self, const void* context, PropertyInfo::GetterOutFn out) -> void {
				const PropT& value = getter(static_cast<const T*>(self), context);
				out(value);
			};
		}

		template <typename PropT, typename SetFn>
		static auto create_setter(SetFn setter)
		{
			return [setter](void* self, void* context, const void* value) -> void {
				setter(static_cast<T*>(self), context, static_cast<const PropT*>(value));
			};
		}

		template <typename PropT, typename GetFn, typename SetFn>
		static auto create_mutate(GetFn getter, SetFn setter)
		{
			return [getter, setter](void* self, void* context, PropertyInfo::MutatorFn mutator) -> void {
				PropT prop = getter(static_cast<const T*>(self), context);
				mutator(prop);
				setter(static_cast<T*>(self), context, &prop);
			};
		}

		template <typename FieldT>
		static auto create_field_getter(FieldT T::*field)
		{
			return [field](const void* self, const void* /*context*/, PropertyInfo::GetterOutFn out) -> void {
				out(static_cast<const T*>(self)->*field);
			};
		}

		template <typename FieldT>
		static auto create_field_setter(FieldT T::*field)
		{
			return [field](void* self, void* /*context*/, const void* value) -> void {
				static_cast<T*>(self)->*field = *static_cast<const FieldT*>(value);
			};
		}

		template <typename FieldT>
		static auto create_field_mutate(FieldT T::*field)
		{
			return [field](void* self, void* /*context*/, PropertyInfo::MutatorFn mutate) -> void {
				mutate(static_cast<T*>(self)->*field);
			};
		}

		template <typename F>
		auto set_init() -> std::enable_if_t<std::is_default_constructible<F>::value>
		{
			result.init = [](void* addr) -> void {
				new(addr) F();
			};
		}

		template <typename F>
		auto set_init() -> std::enable_if_t<!std::is_default_constructible<F>::value>
		{
		}

		template <typename F>
		auto set_copy_init() -> std::enable_if_t<std::is_copy_constructible<F>::value>
		{
			result.copy_init = [](void* addr, const void* copy) -> void {
				new(addr) F(*static_cast<const F*>(copy));
			};
		}

		template <typename F>
		auto set_copy_init() -> std::enable_if_t<!std::is_copy_constructible<F>::value>
		{
		}

		template <typename F>
		auto set_move_init() -> std::enable_if_t<std::is_move_constructible<F>::value>
		{
			result.move_init = [](void* addr, void* move) -> void {
				new(addr) T(std::move(*static_cast<T*>(move)));
			};
		}

		template <typename F>
		auto set_move_init() -> std::enable_if_t<!std::is_move_constructible<F>::value>
		{
		}

		template <typename F>
		auto set_copy_assign() -> std::enable_if_t<std::is_copy_assignable<F>::value>
		{
			result.copy_assign = [](void* self, const void* copy) -> void {
				*static_cast<T*>(self) = *static_cast<const T*>(copy);
			};
		}

		template <typename F>
		auto set_copy_assign() -> std::enable_if_t<!std::is_copy_assignable<F>::value>
		{
		}

		template <typename F>
		auto set_move_assign() -> std::enable_if_t<std::is_move_assignable<F>::value>
		{
			result.move_assign = [](void* self, void* move) -> void {
				*static_cast<T*>(self) = std::move(*static_cast<T*>(move));
			};
		}

		template <typename F>
		auto set_move_assign() -> std::enable_if_t<!std::is_move_assignable<F>::value>
		{
		}

		template <typename F>
		auto set_drop() -> std::enable_if_t<std::is_destructible<F>::value>
		{
			result.drop = [](void* self) -> void {
				static_cast<T*>(self)->~T();
			};
		}

		template <typename F>
		auto set_drop() -> std::enable_if_t<!std::is_destructible<F>::value>
		{
		}

		//////////////////
		///   Fields   ///
	public:

		TypeInfo::Data result;
	};

	template <class I>
	struct InterfaceInfoBuilder final
	{
		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfoBuilder(std::string name)
		{
			result.name = std::move(name);
		}

		//////////////////
		///   Fields   ///
	public:

		InterfaceInfo::Data result;

		///////////////////
		///   Methods   ///
	public:

		template <typename T>
		InterfaceInfoBuilder&& implemented_for()
		{
			static const I vtable = I::template get_vtable<T>();
			result.implementations.insert(std::make_pair(&get_type<T>(), &vtable));
			return std::move(*this);
		}
	};
}

//////////////////
///   Macros   ///

/* Use the macro inside the definition of a type in order for it to be recognized by the reflection system. */
#define SGE_REFLECTED_TYPE			static const ::sge::TypeInfo type_info; const ::sge::TypeInfo& get_type() const { return type_info; }

/* Use this macro in the source file for a type, in order to deine its reflection data. */
#define SGE_REFLECT_TYPE(TYPE)		const ::sge::TypeInfo TYPE::type_info = ::sge::TypeInfoBuilder<TYPE>(#TYPE)

/* Use this macro in the definition of an interface, in order for it to be recorgnized by the reflection system. */
#define SGE_REFLECTED_INTERFACE			static const ::sge::InterfaceInfo interface_info;

/* Use this macro in the source file for an interface, in order to define its pre-existing implementations. */
#define SGE_REFLECT_INTERFACE(INTERF)	const ::sge::InterfaceInfo INTERF::interface_info = ::sge::InterfaceInfoBuilder<INTERF>(#INTERF)

/* Internal macros, used by VTABLE macros. */
#define SGE_VTABLE_BEGIN(INTERF)	template <typename T> static INTERF get_vtable() { INTERF vtable; using ImplT = ::sge::Impl<INTERF, T>;
#define SGE_VTABLE_END				return vtable; }
#define SGE_VTABLE_FN(FUNC)			vtable.FUNC = &ImplT::FUNC;

/* Use these macros in the definition of an interface, where 'INTERF' is name of the interface type, and the proceeding arguments are
* the names of the interface functions to be automatically registered from the 'Impl' specialization.
* Use 'SGE_VTABLE_X' where 'X' is the number of function arguments given. */
#define SGE_VTABLE_0(INTERF, ...)		SGE_VTABLE_BEGIN(INTERF) __VA_ARGS__ SGE_VTABLE_END
#define SGE_VTABLE_1(INTERF, F1, ...)	SGE_VTABLE_0(INTERF, __VA_ARGS__ SGE_VTABLE_FN(F1))
#define SGE_VTABLE_2(INTERF, F1, ...)	SGE_VTABLE_1(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_3(INTERF, F1, ...)	SGE_VTABLE_2(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_4(INTERF, F1, ...)	SGE_VTABLE_3(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_5(INTERF, F1, ...)	SGE_VTABLE_4(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_6(INTERF, F1, ...)	SGE_VTABLE_5(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_7(INTERF, F1, ...)	SGE_VTABLE_6(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
#define SGE_VTABLE_8(INTERF, F1, ...)	SGE_VTABLE_7(INTERF, __VA_ARGS__, SGE_VTABLE_FN(F1))
