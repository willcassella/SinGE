// JsForeignObject.cpp

#include <Core/Memory/Functions.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/ArgAny.h>
#include <Core/Reflection/ConstructorInfo.h>
#include "../private/Util.h"
#include "../private/JsForeignObject.h"

namespace sge
{
	/**
	 * \brief 'Finalize' callback used for JsForeignObject.
	 */
	static void CALLBACK drop_foreign_object(void* ptr)
	{
		auto* foreignObject = static_cast<JsForeignObject*>(ptr);
		foreignObject->type().drop(foreignObject->object());
		sge::free(foreignObject);
	}

	JsForeignObject* JsForeignObject::constructor_init(
		JsValueRef* out,
		const TypeInfo& type,
		JsValueRef proto,
		const ConstructorInfo& constructor,
		JsValueRef* args)
	{
		// Allocate arguments on stack
		auto* convArgs = SGE_STACK_ALLOC(ArgAny, constructor.arg_types().size());

		bool argsGood = true;
		for (std::size_t i = 0; i < constructor.arg_types().size(); ++i)
		{
			argsGood = argsGood && to_native_arg(convArgs + i, args[i], *constructor.arg_types()[i]);
		}

		if (!argsGood)
		{
			return nullptr;
		}

		auto* foreignObject = create(out, type, proto);
		constructor.invoke(foreignObject->object(), convArgs);

		return foreignObject;
	}

	JsForeignObject* JsForeignObject::default_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto)
	{
		auto* object = create(out, type, proto);
		type.init(object->object());

		return object;
	}

	JsForeignObject* JsForeignObject::copy_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto, const void* copy)
	{
		auto* foreignObject = create(out, type, proto);
		type.copy_init(foreignObject->object(), copy);

		return foreignObject;
	}

	JsForeignObject* JsForeignObject::move_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto, void* move)
	{
		auto* foreignObject = create(out, type, proto);
		type.move_init(foreignObject->object(), move);

		return foreignObject;
	}

	JsForeignObject* JsForeignObject::from_jsref(JsValueRef object)
	{
		void* foreignObject;
		JsGetExternalData(object, &foreignObject);
		return static_cast<JsForeignObject*>(foreignObject);
	}

	JsForeignObject* JsForeignObject::create(JsValueRef* out, const TypeInfo& type, JsValueRef proto)
	{
		// Initialize the ForeignObject
		auto* nativeObject = static_cast<JsForeignObject*>(sge::malloc(sizeof(JsForeignObject) + type.size()));
		nativeObject->_type = &type;

		// Initialize the JS object
		JsCreateExternalObject(nativeObject, &drop_foreign_object, out);
		JsSetPrototype(*out, proto);

		return nativeObject;
	}
}

