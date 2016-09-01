// Reflection.h
#pragma once

#include <map>
#include "../STDE/TypeTraits.h"
#include "TypeInfo.h"
#include "InterfaceInfo.h"

namespace sge
{
	namespace specialized
	{
		/* Generic implementation of 'GetType' */
		template <typename T>
		struct GetType final
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
		struct SGE_CORE_API GetType < char > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < byte > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int16 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint16 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int32 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint32 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < int64 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < uint64 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type(...)
			{
				return type_info;
			}
		};

		template <>
		struct SGE_CORE_API GetType < float > final
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
		struct SGE_CORE_API GetType < long double > final
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
