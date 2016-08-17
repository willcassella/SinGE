// TypeTraits.h
#pragma once

#include <type_traits>

namespace stde 
{
	/* Copies the const qualifier from 'FromT' to 'ToT' */
	template <typename FromT, typename ToT>
	struct copy_const
	{
		using type = ToT;
	};

	/* Copies the const qualifier from 'FromT' to 'ToT' */
	template <typename FromT, typename ToT>
	struct copy_const <const FromT, ToT>
	{
		using type = const ToT;
	};

	/* Copies the const qualifier from 'FromT' to 'ToT' */
	template <typename FromT, typename ToT>
	using copy_const_t = typename copy_const<FromT, ToT>::type;
}
