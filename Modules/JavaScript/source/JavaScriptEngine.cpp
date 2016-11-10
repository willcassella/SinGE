// JavaScriptEngine.cpp

#include <iostream>
#include <fstream>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Engine/Scene.h>
#include "../include/JavaScript/JavaScriptEngine.h"
#include "../private/JavaScriptEngineState.h"
#include "../private/JsForeignObject.h"
#include "../private/JsForeignType.h"
#include "../private/JsForeignProperty.h"
#include "../private/JsForeignConstructor.h"
#include "../private/JsTypeInfo.h"
#include "../private/Util.h"

SGE_REFLECT_TYPE(sge::JavaScriptEngine)
.flags(TF_SCRIPT_NOCONSTRUCT);

namespace sge
{
	JsValueRef CALLBACK js_print(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* /*cbState*/)
	{
		for (unsigned short i = 1; i < argc; ++i)
		{
			// Convert argument to JavaScript string
			JsValueRef jsString;
			JsConvertValueToString(args[i], &jsString);

			// Get string pointer and print
			const wchar_t* str;
			std::size_t len;
			JsStringToPointer(jsString, &str, &len);
			std::wcout << str;
		}

		return JS_INVALID_REFERENCE;
	}

	JsValueRef CALLBACK js_load_script(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* cbState)
	{
		if (argc < 2)
		{
			return JS_INVALID_REFERENCE;
		}

		JsValueRef string;
		JsConvertValueToString(args[1], &string);

		char* str;
		std::size_t len;
		JsStringToPointerUtf8Copy(string, &str, &len);

		auto* jsEngine = static_cast<JavaScriptEngine::State*>(cbState);
		jsEngine->load_script(str);

		JsStringFree(str);
		return JS_INVALID_REFERENCE;
	}

	JsValueRef CALLBACK js_native_toString(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short /*argc*/,
		void* /*cbState*/)
	{
		constexpr wchar_t str[] = L"Native Object";

		JsValueRef result;
		JsPointerToString(str, sizeof(str) / sizeof(wchar_t), &result);
		return result;
	}

	JsValueRef CALLBACK js_new_type(
		JsValueRef /*calle*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* cbState)
	{
		auto* jsEngine = static_cast<JavaScriptEngine::State*>(cbState);

		if (argc < 4)
		{
			return JS_INVALID_REFERENCE;
		}

		// Get the name of the type
		char* name;
		std::size_t len;
		JsStringToPointerUtf8Copy(args[1], &name, &len);

		// Create the type data
		JsTypeInfo::Data type_data;
		type_data.context = jsEngine->js_runtime.context;
		type_data.constructor = args[2];
		type_data.prototype = args[3];

		jsEngine->js_types.insert(std::make_pair(name, std::make_unique<JsTypeInfo>(name, std::move(type_data))));

		JsStringFree(name);
		return JS_INVALID_REFERENCE;
	}

	JsValueRef CALLBACK js_process_entities_mut(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* cbState)
	{
		if (argc < 3)
		{
			return JS_INVALID_REFERENCE;
		}

		auto* jsEngine = static_cast<JavaScriptEngine::State*>(cbState);
		auto* types = SGE_STACK_ALLOC(const TypeInfo*, argc - 2);

		for (unsigned short i = 2; i < argc; ++i)
		{
			// Get the type name argument
			char* typeName;
			std::size_t len;
			JsStringToPointerUtf8Copy(args[i], &typeName, &len);

			// Search for the type, stopping if not found
			auto* type = jsEngine->scene->get_component_type(typeName);
			JsStringFree(typeName);

			if (!type)
			{
				return JS_INVALID_REFERENCE;
			}

			types[i - 2] = type;
		}

		auto processFn = [args](ProcessingFrameMut& pframe, EntityId entity, ComponentInterface* const components[]) {
			JsValueRef funcArgs[] = { args[0], JS_INVALID_REFERENCE };
			JsIntToNumber(static_cast<int>(entity), funcArgs + 1);
			JsCallFunction(args[1], funcArgs, 2, nullptr);
		};

		jsEngine->scene->process_entities_mut(types, argc - 2, processFn);
		return JS_INVALID_REFERENCE;
	}

	JsValueRef CALLBACK js_new_object(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* cbState)
	{
		if (argc < 2)
		{
			return JS_INVALID_REFERENCE;
		}

		auto* jsEngine = static_cast<JavaScriptEngine::State*>(cbState);

		// Find the type referred to by this call
		char* name;
		std::size_t len;
		JsStringToPointerUtf8Copy(args[1], &name, &len);
		auto typeIter = jsEngine->js_types.find(name);
		JsStringFree(name);

		// If the type could not be found
		if (typeIter == jsEngine->js_types.end())
		{
			return JS_INVALID_REFERENCE;
		}

		JsValueRef result;
		if (argc < 3)
		{
			typeIter->second->init(&result);
		}
		else
		{
			typeIter->second->copy_init(&result, args + 2);
		}

		return result;
	}

	JavaScriptEngine::JavaScriptEngine(Scene& scene)
	{
		_state = std::make_unique<State>();
		_state->scene = &scene;

		// Create a runtime
		JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &_state->js_runtime.runtime);

		// Create an execution context
		JsCreateContext(_state->js_runtime.runtime, &_state->js_runtime.context);
		JsSetCurrentContext(_state->js_runtime.context);

		// Get the global object
		JsGetGlobalObject(&_state->js_environment.global_object);

		// Create the base prototype
		JsCreateObject(&_state->base_foreign_prototype);
		JsAddRef(_state->base_foreign_prototype, nullptr);

		// Create the base prototype toString function
		JsPropertyIdRef toStringPropId;
		JsValueRef toStringFn;
		JsGetPropertyIdFromNameUtf8("toString", &toStringPropId);
		JsCreateFunction(&js_native_toString, nullptr, &toStringFn);
		JsSetProperty(_state->base_foreign_prototype, toStringPropId, toStringFn, true);

		// Create get the true and false values
		JsGetTrueValue(&_state->js_environment.true_value);
		JsGetFalseValue(&_state->js_environment.false_value);

		// Create the property prototype
		JsCreateObject(&_state->js_property.property_prototype);
		JsAddRef(_state->js_property.property_prototype, nullptr);

		// All native properties are not configurable
		JsPropertyIdRef configurablePropID;
		JsGetPropertyIdFromNameUtf8("configurable", &configurablePropID);
		JsSetProperty(_state->js_property.property_prototype, configurablePropID, _state->js_environment.false_value, true);

		// All native properties are enumerable
		JsPropertyIdRef enumerablePropID;
		JsGetPropertyIdFromNameUtf8("enumerable", &enumerablePropID);
		JsSetProperty(_state->js_property.property_prototype, enumerablePropID, _state->js_environment.true_value, true);

		// Get the 'get' and 'set' property IDs
		JsGetPropertyIdFromNameUtf8("get", &_state->js_property.get_prop_id);
		JsGetPropertyIdFromNameUtf8("set", &_state->js_property.set_prop_id);

		// Create 'print' function
		JsPropertyIdRef printPropId;
		JsValueRef printFn;
		JsGetPropertyIdFromNameUtf8("print", &printPropId);
		JsCreateFunction(&js_print, nullptr, &printFn);
		JsSetProperty(_state->js_environment.global_object, printPropId, printFn, true);

		// Create 'load_script' function
		JsPropertyIdRef loadScriptPropId;
		JsValueRef loadScriptFn;
		JsGetPropertyIdFromNameUtf8("load_script", &loadScriptPropId);
		JsCreateFunction(&js_load_script, _state.get(), &loadScriptFn);
		JsSetProperty(_state->js_environment.global_object, loadScriptPropId, loadScriptFn, true);

		// Create 'new_type' function
		JsPropertyIdRef newTypePropId;
		JsValueRef newTypeFn;
		JsGetPropertyIdFromNameUtf8("new_type", &newTypePropId);
		JsCreateFunction(&js_new_type, _state.get(), &newTypeFn);
		JsSetProperty(_state->js_environment.global_object, newTypePropId, newTypeFn, true);

		// Create 'process_entities_mut' function
		JsPropertyIdRef processEntitiesMutPropId;
		JsValueRef processEntitiesMutFn;
		JsGetPropertyIdFromNameUtf8("process_entities_mut", &processEntitiesMutPropId);
		JsCreateFunction(&js_process_entities_mut, _state.get(), &processEntitiesMutFn);
		JsSetProperty(_state->js_environment.global_object, processEntitiesMutPropId, processEntitiesMutFn, true);

		// Create 'new_object' function
		JsPropertyIdRef newObjectPropId;
		JsValueRef newObjectFn;
		JsGetPropertyIdFromNameUtf8("new_object", &newObjectPropId);
		JsCreateFunction(&js_new_object, _state.get(), &newObjectFn);
		JsSetProperty(_state->js_environment.global_object, newObjectPropId, newObjectFn, true);
	}

	JavaScriptEngine::~JavaScriptEngine()
	{
		if (_state)
		{
			// Dispose runtime
			JsSetCurrentContext(JS_INVALID_REFERENCE);
			JsDisposeRuntime(_state->js_runtime.runtime);
		}
	}

	void JavaScriptEngine::register_type(const TypeInfo& type)
	{
		JsSetCurrentContext(_state->js_runtime.context);

		auto foreignType = JsForeignType::create(
			_state->js_property.property_prototype,
			_state->js_property.get_prop_id,
			_state->js_property.set_prop_id,
			type);

		// Generate a name for the type, and put it in the proper scope
		JsValueRef parent = _state->js_environment.global_object;
		std::size_t nameStart = 0;
		for (std::size_t i = 0; i < type.name().size(); ++i)
		{
			if (type.name()[i] == ':')
			{
				JsPropertyIdRef scopeName;
				JsGetPropertyIdFromNameUtf8(type.name().substr(nameStart, i).c_str(), &scopeName);
				parent = get_or_create_property(parent, scopeName);

				i += 1;
				nameStart = i + 1;
			}
		}

		// Register the constructor function as a global
		JsPropertyIdRef namePropId = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8(type.name().c_str() + nameStart, &namePropId);
		JsSetProperty(parent, namePropId, foreignType->js_constructor(), true);

		// Add property to set of registerd types
		_state->registered_foreign_types[&type] = std::move(foreignType);
	}

	void JavaScriptEngine::run_expression(const char* expr)
	{
		JsSetCurrentContext(_state->js_runtime.context);

		// Run the expression
		JsValueRef result;
		JsRunScriptUtf8(expr, _state->js_runtime.source_context++, "", &result);
	}

	void JavaScriptEngine::load_script(const char* path)
	{
		_state->load_script(path);
	}
}
