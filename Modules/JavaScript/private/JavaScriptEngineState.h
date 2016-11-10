// JavaScriptEngineState.h
#pragma once

#include <ChakraCore.h>
#include "../include/JavaScript/JavaScriptEngine.h"
#include "Util.h"
#include "JsForeignType.h"

namespace sge
{
	struct JsForeignType;
	struct JsTypeInfo;

	struct JavaScriptEngine::State
	{
		///////////////////
		///   Methods   ///
	public:

		void load_script(const char* path)
		{
			std::wifstream file(path);

			if (!file)
			{
				return;
			}

			file.seekg(0, std::ios::end);
			auto length = file.tellg();
			file.seekg(0, std::ios::beg);

			std::vector<wchar_t> contents(length);
			file.read(&contents.front(), length);

			auto error = JsRunScript(&contents.front(), js_runtime.source_context++, L"", nullptr);

			if (error == JsErrorScriptException)
			{
				// Get the exception
				JsValueRef exception = JS_INVALID_REFERENCE;
				JsGetAndClearException(&exception);

				// Get the exception as a string
				JsValueRef exceptionString = JS_INVALID_REFERENCE;
				JsConvertValueToString(exception, &exceptionString);
				const wchar_t* str = nullptr;
				std::size_t len = 0;
				JsStringToPointer(exceptionString, &str, &len);

				// Print it
				std::wcout << L"Exception thrown: ";
				std::wcout.write(str, len);
				std::wcout << std::endl;
			}
		}

		//////////////////
		///   Fields   ///
	public:

		/* Game Engine state. */
		Scene* scene;

		/* JavaScript runtime state. */
		struct Runtime
		{
			JsRuntimeHandle runtime;
			JsContextRef context;
			JsSourceContext source_context;
		} js_runtime;

		/* JavaScript environment global state. */
		struct Environment
		{
			JsValueRef global_object;
			JsValueRef null_value;
			JsValueRef undefined_value;
			JsValueRef true_value;
			JsValueRef false_value;
			JsPropertyIdRef length_prop_id;
		} js_environment;

		/* JavaScript environment state pertinent to properties. */
		struct Property
		{
			JsValueRef property_prototype;
			JsPropertyIdRef get_prop_id;
			JsPropertyIdRef set_prop_id;
		} js_property;

		JsValueRef base_foreign_prototype;
		std::unordered_map<const TypeInfo*, std::unique_ptr<JsForeignType>> registered_foreign_types;
		std::unordered_map<std::string, std::unique_ptr<JsTypeInfo>> js_types;
	};
}
