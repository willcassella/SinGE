// JsForeignConstructor.cpp

#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/ConstructorInfo.h>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ArgAny.h>
#include "../private/JsForeignConstructor.h"
#include "../private/JsForeignObject.h"
#include "../private/Util.h"

namespace sge
{
    static JsValueRef construct_foreign_object(
        const TypeInfo& type,
        JsValueRef proto,
        const ConstructorInfo& constructor,
        JsValueRef* args)
    {
        // Allocate arguments on stack
        auto* convArgs = SGE_STACK_ALLOC(ArgAny, constructor.arg_types().size());

        // Convert all arguments
        bool argsGood = true;
        for (std::size_t i = 0; i < constructor.arg_types().size(); ++i)
        {
            argsGood = argsGood && to_native_arg(convArgs + i, args[i], *constructor.arg_types()[i]);
        }

        if (!argsGood)
        {
            return nullptr;
        }

        // Create the foreign object
        JsValueRef result = JsForeignObject::create_js_foreign_object();
        auto* foreignObject = JsForeignObject::heap_init_object(result, type, proto);

        // Call the constructor
        constructor.invoke(foreignObject->object(), convArgs);

        return result;
    }

    static JsValueRef default_construct_foreign_object(const TypeInfo& type, JsValueRef proto)
    {
        // Create the foreign object
        JsValueRef result = JsForeignObject::create_js_foreign_object();
        auto* foreignObject = JsForeignObject::heap_init_object(result, type, proto);

        // Call the constructor
        type.init(foreignObject->object());

        return result;
    }

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
            return default_construct_foreign_object(*state->foreign_type, state->js_proto);
        }

        // Otherwise, we need to find a constructor with the same arity as our number of arguments
        auto ctor = state->foreign_type->find_constructor(argc - 1);
        if (!ctor)
        {
            // No constructor with this number of arguments
            return JS_INVALID_REFERENCE;
        }

        // Construct this object with the given arguments
        return construct_foreign_object(*state->foreign_type, state->js_proto, *ctor, args + 1);
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
        return construct_foreign_object(*state->foreign_type, state->js_proto, *state->foreign_constructor, args + 1);
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
