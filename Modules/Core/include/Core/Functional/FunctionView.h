// FunctionView.h
#pragma once

namespace sge
{
	template <typename Func>
	struct FunctionView;

	template <typename Ret, typename ... Args>
	struct FunctionView< Ret(Args...) >
	{
	private:

		union InvokerData
		{
			void* lambda;
			Ret(*func)(Args...);
		};

		////////////////////////
		///   Constructors   ///
	public:

		/* Creates a FunctionView from a lambda/function pointer.
		* NOTE: This form allows lambdas and function pointers that do not exactly match the signature of this FunctionView.*/
		template <typename F>
		FunctionView(F&& lambda)
		{
			_invoker = [](InvokerData data, Args... args) -> Ret {
				return (*static_cast<std::remove_reference_t<F>*>(data.lambda))(std::forward<Args>(args)...);
			};
			_data.lambda = &lambda;
		}

		/* Creates a FunctionView from a function reference.
		* NOTE: This form only supports function references that exactly match the signature for this FunctionView.
		* If that is not desireable, just use the normal function pointer syntax ('&func' instead of 'func'). */
		FunctionView(Ret(&func)(Args...))
		{
			_invoker = [](InvokerData data, Args... args) -> Ret {
				return data.func(std::forward<Args>(args)...);
			};
			_data.func = &func;
		}

		/* You may not use a function reference that does not exactly match the signature of this FunctionView (use a function pointer instead). */
		template <typename FRet, typename ... FArgs>
		FunctionView(FRet(&)(FArgs...)) = delete;

		FunctionView(FunctionView& copy)
			: _invoker{ copy._invoker }, _data{ copy._data }
		{
		}
		FunctionView(const FunctionView& copy)
			: _invoker{ copy._invoker }, _data{ copy._data }
		{
		}
		FunctionView(FunctionView&& move)
			: _invoker{ move._invoker }, _data{ move._data }
		{
		}
		FunctionView(const FunctionView&& move)
			: _invoker{ move._invoker }, _data{ _move.data }
		{
		}

		///////////////////
		///   Methods   ///
	public:

		Ret invoke(Args... args) const
		{
			return _invoker(_data, std::forward<Args>(args)...);
		}

		/////////////////////
		///   Operators   ///
	public:

		Ret operator()(Args... args) const
		{
			return invoke(std::forward<Args>(args)...);
		}

		////////////////
		///   Data   ///
	private:

		Ret(*_invoker)(InvokerData, Args...);
		InvokerData _data;
	};
}
