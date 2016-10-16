// TMP.h
#pragma once

namespace sge
{
	namespace tmp
	{
		/* Represents a parameter pack. Useful for function overloading type stuff.
		 * I used to have scheme-style pairs here too, but this was a lot more useful and is probably more efficient (compile-time). */
		template <typename ... T>
		struct list
		{
		};

		namespace impl
		{
			template <typename SeqT>
			struct car;

			template <typename SeqT>
			struct cdr;

			template <typename SeqT>
			struct len;

			/* Implements 'car' for a list. */
			template <typename CarT, typename ... CdrTs>
			struct car< list<CarT, CdrTs...> >
			{
				using result_t = CarT;
			};

			/* Implements 'cdr' for a list. */
			template <typename CarT, typename ... CdrTs>
			struct cdr< list<CarT, CdrTs...> >
			{
				using result_t = list<CdrTs...>;
			};

			/* Implements 'len' for a list. */
			template <typename ... Ts>
			struct len< list<Ts...> >
			{
				static constexpr std::size_t result_v = sizeof...(Ts);
			};

			/* Implements 'cdr_n'. */
			template <typename SeqT, std::size_t N>
			struct cdr_n
			{
				using result_t = typename cdr_n<typename cdr<SeqT>::result_t, N - 1>::result_t;
			};

			/* Ends 'cdr_n' for list. */
			template <typename SeqT>
			struct cdr_n< SeqT, 0 >
			{
				using result_t = SeqT;
			};
		}

		/* Equivalent to scheme 'car' function. */
		template <typename SeqT>
		using car = typename impl::car<SeqT>::result_t;

		/* Equivalent to scheme 'cdr' function. */
		template <typename SeqT>
		using cdr = typename impl::cdr<SeqT>::result_t;

		/* Returns the length of a sequence. */
		template <typename SeqT>
		constexpr std::size_t len = impl::len<SeqT>::result_v;

		/* Equivalent to calling 'cdr' N times. */
		template <typename SeqT, std::size_t N>
		using cdr_n = typename impl::cdr_n<SeqT, N>::result_t;

		/* Equivalent to calling 'cdr' N times, followed by 'car'. */
		template <typename SeqT, std::size_t N>
		using car_n = car<cdr_n<SeqT, N>>;
	}
}