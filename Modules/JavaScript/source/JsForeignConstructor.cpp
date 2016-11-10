// JsForeignConstructor.cpp

#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/ConstructorInfo.h>
#include "../private/JsForeignConstructor.h"
#include "../private/JsForeignObject.h"

namespace sge
{
	static JsValueRef CALLBACK js_foreign_constructor(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* callbackState)
	{
		auto* state = static_cast<const JsForeignConstructorState*>(callbackState);

		// If we only have one argument ('this'), just use the default constructor
		if (argc == 1)
		{
			JsValueRef object = JS_INVALID_REFERENCE;
			JsForeignObject::default_init(&object, *state->foreign_type, state->js_proto);

			return object;
		}

		// Otherwise, we need to find a constructor with the same arity as our number of arguments
		auto ctor = state->foreign_type->find_constructor(argc - 1);
		if (!ctor)
		{
			// No constructor with this number of arguments
			return JS_INVALID_REFERENCE;
		}

		// Try to construct this object with the given arguments
		JsValueRef object = JS_INVALID_REFERENCE;
		JsForeignObject::constructor_init(&object, *state->foreign_type, state->js_proto, *ctor, args + 1);

		return object;
	}

	static JsValueRef CALLBACK js_foreign_named_constructor(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* callbackState)
	{
		auto* state = static_cast<const JsForeignNamedConstructorState*>(callbackState);

		// Make sure the number of arguments given matches the number of arguments required for our constructor
		if (argc - 1 != state->foreign_constructor->arg_types().size())
		{
			return JS_INVALID_REFERENCE;
		}

		// Try to construct this object with the given arguments
		JsValueRef object = JS_INVALID_REFERENCE;
		JsForeignObject::constructor_init(&object, *state->foreign_type, state->js_proto, *state->foreign_constructor, args + 1);

		return object;
	}

	JsValueRef init_foreign_constructor(
		JsForeignConstructorState& constructorState,
		const TypeInfo& foreignType,
		JsValueRef jsPrototype)
	{
		// Initialize the constructor state
		constructorState.foreign_type = &foreignType;
		constructorState.js_proto = jsPrototype;

		// Create the constructor
		JsValueRef jsConstructor;
		JsCreateFunction(&js_foreign_constructor, &constructorState, &jsConstructor);
		return jsConstructor;
	}

	void init_foreign_named_constructor(
		JsForeignNamedConstructorState& constructorState,
		const TypeInfo& foreignType,
		JsValueRef jsPrototype,
		JsValueRef jsConstructor,
		const ConstructorInfo& foreignConstructor,
		const std::string& name)
	{
		// Initialze the constructor state
		constructorState.foreign_type = &foreignType;
		constructorState.js_proto = jsPrototype;
		constructorState.foreign_constructor = &foreignConstructor;

		// Create the constructor
		JsValueRef jsNamedConstructor;
		JsCreateFunction(js_foreign_named_constructor, &constructorState, &jsNamedConstructor);

		// Create the named constructor property
		JsPropertyIdRef namedConstructorPropId;
		JsGetPropertyIdFromNameUtf8(name.c_str(), &namedConstructorPropId);
		JsSetProperty(jsConstructor, namedConstructorPropId, jsNamedConstructor, true);
	}
}
