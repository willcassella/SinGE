// JavaScriptEngine.h
#pragma once

#include <memory>
#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	struct Scene;

	struct SGE_JAVASCRIPT_API JavaScriptEngine
	{
		SGE_REFLECTED_TYPE;
		struct State;

		////////////////////////
		///   Constructors   ///
	public:

		JavaScriptEngine(Scene& scene);
		~JavaScriptEngine();

		///////////////////
		///   Methods   ///
	public:

		void register_type(const TypeInfo& type);

		void run_expression(const char* expr);

		void load_script(const char* path);

		//////////////////
		///   Fields   ///
	private:

		std::unique_ptr<State> _state;
	};
}
