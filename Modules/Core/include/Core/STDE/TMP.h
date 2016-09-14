// TMP.h
#pragma once

#include <tuple>

namespace stde
{
	template <typename T>
	struct type
	{
	};

	template <typename ... T>
	struct type_sequence 
	{
		template <template <typename ... F> typename Template>
		using expand_into = Template<T...>;

		template <std::size_t i>
		using at = typename std::tuple_element<i, std::tuple<T...>>::type;

		static constexpr std::size_t size = sizeof...(T);
	};
}
