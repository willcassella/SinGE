// FunctionView.h
#pragma once

namespace sge
{
	template <typename Func>
	struct FunctionView;

	template <typename Ret, typename ... Args>
	struct FunctionView< Ret(Args...) >
	{
		////////////////////////
		///   Constructors   ///
	public:

		template <typename F>
		FunctionView(F&& func)
		{
			_invoker = [](void* f, Args... args) -> Ret {
				return static_cast<std::remove_reference_t<F>*>(f)(std::forward<Args>(args)...);
			};
			_func = &func;
		}

		///////////////////
		///   Methods   ///
	public:

		Ret invoke(Args... args) const
		{
			return _invoker(_func, std::forward<Args>(args)...);
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

		Ret(*_invoker)(void*, Args...);
		void* _func;
	};
}
