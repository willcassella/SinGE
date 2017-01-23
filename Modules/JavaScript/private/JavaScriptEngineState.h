// JavaScriptEngineState.h
#pragma once

#include <ChakraCore.h>
#include "../include/JavaScript/JavaScriptEngine.h"

namespace sge
{
	struct JsForeignType;
	struct JsTypeInfo;
	struct Scene;
	struct SystemFrame;

	struct JavaScriptEngine::State
	{
		///////////////////
		///   Methods   ///
	public:

		void load_script(const char* path);

		void register_foreign_type(const TypeInfo& type);

		void register_js_type(const char* name, JsValueRef constructor, JsValueRef proto);

		JsValueRef get_js_prototype(const TypeInfo& type);

		//////////////////
		///   Fields   ///
	public:

		/* Game Engine state. */
		Scene* scene;
		SystemFrame* frame;

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
		std::unordered_map<const TypeInfo*, JsValueRef> js_prototypes;
	};
}
