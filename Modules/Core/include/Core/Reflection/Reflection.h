// Reflection.h
#pragma once

#include <map>
#include "../STDE/TypeTraits.h"
#include "TypeInfo.h"
#include "InterfaceInfo.h"

namespace singe
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
		};

		/* Generic implementation of 'GetInterface' */
		template <typename T>
		struct GetInterface final
		{
			static const InterfaceInfo& get_interface()
			{
				return T::interface_info;
			}
		};

		template <>
		struct CORE_API GetType < char > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < byte > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < int16 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < uint16 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < int32 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < uint32 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < int64 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < uint64 > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < float > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < double > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};

		template <>
		struct CORE_API GetType < long double > final
		{
			static const TypeInfo type_info;

			static const TypeInfo& get_type()
			{
				return type_info;
			}
		};
	}

	/////////////////////
	///   Functions   ///

	template <typename T>
	const TypeInfo& get_type()
	{
		return specialized::GetType<T>::get_type();
	}

	template <typename T>
	const InterfaceInfo& get_interface()
	{
		return specialized::GetInterface<T>::get_interface();
	}
}

//////////////////
///   Macros   ///

#define REFLECTED_TYPE			static const ::singe::TypeInfo type_info
#define REFLECT_TYPE(T)			const ::singe::TypeInfo T::type_info = ::singe::TypeInfoBuilder<T>(#T)

#define REFLECTED_INTERFACE		static const ::singe::InterfaceInfo interface_info
#define REFLECT_INTERFACE(T)	const ::singe::InterfaceInfo T::interface_info = ::singe::InterfaceInfoBuilder<T>(#T)

#define AUTO_IMPL_BEGIN(InterfaceT) template <typename T> static InterfaceT auto_impl() { InterfaceT result;
#define AUTO_IMPL_END return result; }

#define IMPL(method)																		\
	result.method = [](auto* self, auto&& ... args) -> decltype(auto) {						\
		using TargetT = ::stde::copy_const_t<std::remove_pointer_t<decltype(self)>, T>;		\
		return static_cast<TargetT*>(self)->method(std::forward<decltype(args)>(args)...);	\
	};

#define AUTO_IMPL_0(InterfaceT) AUTO_IMPL_BEGIN(InterfaceT) AUTO_IMPL_END
#define AUTO_IMPL_1(InterfaceT, m1) AUTO_IMPL_BEGIN(InterfaceT) IMPL(m1) AUTO_IMPL_END
#define AUTO_IMPL_2(InterfaceT, m1, m2) AUTO_IMPL_BEGIN(InterfaceT) IMPL(m1) IMPL(m2) AUTO_IMPL_END
#define AUTO_IMPL_3(InterfaceT, m1, m2, m3) AUTO_IMPL_BEGIN(InterfaceT) IMPL(m1) IMPL(m2) IMPL(m2) AUTO_IMPL_END
