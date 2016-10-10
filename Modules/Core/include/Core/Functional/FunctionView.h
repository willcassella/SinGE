// FunctionView.h
#pragma once

#include <utility>
#include <type_traits>

namespace sge
{
	template <typename Fn>
	struct FunctionView;

	template <typename RetT, typename ... ArgTs>
	struct FunctionView< RetT(ArgTs...) >
	{
	private:

		union UserData
		{
			void* state;
			RetT(*func)(ArgTs...);
		};
		using Function = RetT(UserData, ArgTs...);

		////////////////////////
		///   Constructors   ///
	public:

		/* Explicitly initializes this FunctionView with a state pointer and function pointer. */
		FunctionView(void* state, Function* func)
		{
			_data.state = state;
			_func = func;
		}

		/* Creates a FunctionView from a function reference.
		* NOTE: This form only supports function references that exactly match the signature for this FunctionView.
		* If that is not desireable, just use the normal function pointer syntax ('&func' instead of 'func'). */
		FunctionView(RetT(&func)(ArgTs...))
		{
			_data.func = &func;
			_func = [](UserData data, ArgTs... args) -> RetT {
				return data.func(std::forward<ArgTs>(args)...);
			};
		}

		/* Creates a FunctionView from a lambda/function pointer.
		* NOTE: This form allows lambdas and function pointers that do not exactly match the signature of this FunctionView.*/
		template <typename Fn>
		FunctionView(Fn&& lambda)
		{
			_data.state = &lambda;
			_func = [](UserData data, ArgTs... args) -> RetT {
				return (*static_cast<std::remove_reference_t<Fn>*>(data.state))(std::forward<ArgTs>(args)...);
			};
		}

		/* You may not use a function reference that does not exactly match the signature of this FunctionView (use a function pointer instead). */
		template <typename FRetT, typename ... FArgTs>
		FunctionView(FRetT(&)(FArgTs...)) = delete;

		FunctionView(FunctionView& copy)
			: _data(copy._data), _func(copy._func)
		{
		}
		FunctionView(const FunctionView& copy)
			: _data(copy._data), _func(copy._func)
		{
		}
		FunctionView(FunctionView&& move)
			: _data(move._data), _func()
		{
		}
		FunctionView(const FunctionView&& move)
			: _data(move._data), _func(move._func)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		RetT invoke(ArgTs... args) const
		{
			return _func(_data, std::forward<ArgTs>(args)...);
		}

		/////////////////////
		///   Operators   ///
	public:

		RetT operator()(ArgTs... args) const
		{
			return _func(_data, std::forward<ArgTs>(args)...);
		}

		////////////////
		///   Data   ///
	private:

		UserData _data;
		Function* _func;
	};
}
