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
			JsValueRef jsString = JS_INVALID_REFERENCE;
			JsConvertValueToString(args[i], &jsString);

			// Get string pointer and print
			const wchar_t* str = nullptr;
			std::size_t len = 0;
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

		JsValueRef string = JS_INVALID_REFERENCE;
		JsConvertValueToString(args[1], &string);

		char* str = nullptr;
		std::size_t len = 0;
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

		JsValueRef result = JS_INVALID_REFERENCE;
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
		char* name = nullptr;
		std::size_t len = 0;
		JsStringToPointerUtf8Copy(args[1], &name, &len);

		// Register the type
		jsEngine->register_js_type(name, args[2], args[3]);

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

		// Create an array to hold the types
		const int numTypes = argc - 2;
		const TypeInfo** types = SGE_STACK_ALLOC(const TypeInfo*, numTypes);
		JsValueRef* jsProtos = SGE_STACK_ALLOC(JsValueRef, numTypes);

		// Get the types passed to this function
		for (unsigned short i = 1; i < argc - 1; ++i)
		{
			// Get the type name argument
			char* typeName = nullptr;
			std::size_t len = 0;
			JsStringToPointerUtf8Copy(args[i], &typeName, &len);

			// Search for the component type
			auto* type = jsEngine->scene->get_component_type(typeName);
			JsStringFree(typeName);

			// If the type doesn't exist, we can't do anything
			if (!type)
			{
				return JS_INVALID_REFERENCE;
			}

			// Add the type to the types array
			types[i - 1] = type;

			// Get the javascript prototype for this type
			jsProtos[i - 1] = jsEngine->get_js_prototype(*type);
		}

		// Create an array for all the arguments we'll pass to the processing function
		JsValueRef* jsArgs = SGE_STACK_ALLOC(JsValueRef, numTypes + 3);

		// JsArgs inerits the 'this' argument
		jsArgs[0] = args[0];

		// First real argument (pframe) is a foreign object
		jsArgs[1] = jsEngine->js_environment.null_value; // TODO: Initialize real pframe object

		// Initialize all of the component arguments
		for (int i = 0; i < numTypes; ++i)
		{
			jsArgs[i + 3] = JsForeignObject::create_js_foreign_object();
		}

		// Create an array of foreign objects for all of the components
		byte* foreignObjectBuffer = SGE_STACK_ALLOC(byte, JsForeignObject::object_pointer_alloc_size() * numTypes);

		// Wrapper function to call
		auto processFn = [jsProcessFn = args[argc - 1], jsArgs, numTypes, foreignObjectBuffer, types, jsProtos](
			ProcessingFrameMut&,
			EntityId entity,
			ComponentInterface* const components[])
		{
			// Get the entity ID as an int TODO: FIX THIS
			JsIntToNumber(static_cast<int>(entity), &jsArgs[2]);

			// Initialize all of the component arguments
			for (int i = 0; i < numTypes; ++i)
			{
				JsForeignObject::stack_init_pointer(
					&foreignObjectBuffer[JsForeignObject::object_pointer_alloc_size() * i],
					jsArgs[i + 3],
					*types[i],
					jsProtos[i],
					components[i]);
			}

			// Run the processing function
			auto error = JsCallFunction(jsProcessFn, jsArgs, numTypes + 3, nullptr);
		};

		// Run the process function
		jsEngine->scene->process_entities_mut(types, numTypes, processFn);

		// Null out all the js arguments
		for (int i = 0; i < numTypes; ++i)
		{
			JsSetExternalData(jsArgs[i + 3], nullptr);
		}

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
		char* name = nullptr;
		std::size_t len = 0;
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

		// Get environment references
		JsGetGlobalObject(&_state->js_environment.global_object);
		JsGetNullValue(&_state->js_environment.null_value);
		JsGetUndefinedValue(&_state->js_environment.undefined_value);
		JsGetTrueValue(&_state->js_environment.true_value);
		JsGetFalseValue(&_state->js_environment.false_value);

		// Create the base prototype
		JsCreateObject(&_state->base_foreign_prototype);
		JsAddRef(_state->base_foreign_prototype, nullptr);

		// Create the base prototype toString function
		JsPropertyIdRef toStringPropId = JS_INVALID_REFERENCE;
		JsValueRef toStringFn = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("toString", &toStringPropId);
		JsCreateFunction(&js_native_toString, nullptr, &toStringFn);
		JsSetProperty(_state->base_foreign_prototype, toStringPropId, toStringFn, true);

		// Create the property prototype
		JsCreateObject(&_state->js_property.property_prototype);
		JsAddRef(_state->js_property.property_prototype, nullptr);

		// All native properties are not configurable
		JsPropertyIdRef configurablePropID = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("configurable", &configurablePropID);
		JsSetProperty(_state->js_property.property_prototype, configurablePropID, _state->js_environment.false_value, true);

		// All native properties are enumerable
		JsPropertyIdRef enumerablePropID = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("enumerable", &enumerablePropID);
		JsSetProperty(_state->js_property.property_prototype, enumerablePropID, _state->js_environment.true_value, true);

		// Get the 'get' and 'set' property IDs
		JsGetPropertyIdFromNameUtf8("get", &_state->js_property.get_prop_id);
		JsGetPropertyIdFromNameUtf8("set", &_state->js_property.set_prop_id);

		// Create 'print' function
		JsPropertyIdRef printPropId = JS_INVALID_REFERENCE;
		JsValueRef printFn = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("print", &printPropId);
		JsCreateFunction(&js_print, nullptr, &printFn);
		JsSetProperty(_state->js_environment.global_object, printPropId, printFn, true);

		// Create 'load_script' function
		JsPropertyIdRef loadScriptPropId = JS_INVALID_REFERENCE;
		JsValueRef loadScriptFn = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("load_script", &loadScriptPropId);
		JsCreateFunction(&js_load_script, _state.get(), &loadScriptFn);
		JsSetProperty(_state->js_environment.global_object, loadScriptPropId, loadScriptFn, true);

		// Create 'new_type' function
		JsPropertyIdRef newTypePropId = JS_INVALID_REFERENCE;
		JsValueRef newTypeFn = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("new_type", &newTypePropId);
		JsCreateFunction(&js_new_type, _state.get(), &newTypeFn);
		JsSetProperty(_state->js_environment.global_object, newTypePropId, newTypeFn, true);

		// Create 'process_entities_mut' function
		JsPropertyIdRef processEntitiesMutPropId = JS_INVALID_REFERENCE;
		JsValueRef processEntitiesMutFn = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8("process_entities_mut", &processEntitiesMutPropId);
		JsCreateFunction(&js_process_entities_mut, _state.get(), &processEntitiesMutFn);
		JsSetProperty(_state->js_environment.global_object, processEntitiesMutPropId, processEntitiesMutFn, true);

		// Create 'new_object' function
		JsPropertyIdRef newObjectPropId = JS_INVALID_REFERENCE;
		JsValueRef newObjectFn = JS_INVALID_REFERENCE;
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
		_state->register_foreign_type(type);
	}

	void JavaScriptEngine::run_expression(const char* expr)
	{
		JsSetCurrentContext(_state->js_runtime.context);

		// Run the expression
		JsValueRef result = JS_INVALID_REFERENCE;
		JsRunScriptUtf8(expr, _state->js_runtime.source_context++, "", &result);
	}

	void JavaScriptEngine::load_script(const char* path)
	{
		_state->load_script(path);
	}
}
