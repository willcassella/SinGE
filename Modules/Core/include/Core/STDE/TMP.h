// TMP.h
#pragma once

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
	};
}
