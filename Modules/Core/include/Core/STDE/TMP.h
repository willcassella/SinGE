// TMP.h
#pragma once

#include <tuple>

namespace stde
{
	template <typename ... T>
	struct type_sequence
	{
		template <std::size_t i>
		using at = typename std::tuple_element<i, std::tuple<T...>>::type;
	};
}