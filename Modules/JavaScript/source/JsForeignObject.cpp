// JsForeignObject.cpp

#include <Core/Memory/Functions.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/ConstructorInfo.h>
#include "../private/JsForeignObject.h"

namespace sge
{
	void JsForeignObject::stack_init_pointer(void* addr, JsValueRef jsObject, const TypeInfo& type, JsValueRef proto, void* pointer)
	{
		// Initialize the foreign object
		init(addr, jsObject, type, proto, STACK_ALLOCATED | OBJECT_POINTER);

		// Initialize the pointer
		new (static_cast<JsForeignObject*>(addr) + 1) void*{ pointer };
	}

	JsForeignObject* JsForeignObject::heap_init_object(JsValueRef jsObject, const TypeInfo& type, JsValueRef proto)
	{
		// Allocate memory and initialize the foreign object
		auto* foreignObject = static_cast<JsForeignObject*>(sge::malloc(sizeof(JsForeignObject) + type.size()));
		init(foreignObject, jsObject, type, proto, HEAP_ALLOCATED | OBJECT_STORED_CONTIGUOUS);

		return foreignObject;
	}

	void JsForeignObject::drop_foreign_object(void* ptr)
	{
		auto* foreignObject = static_cast<JsForeignObject*>(ptr);

		if (!foreignObject)
		{
			return;
		}

		// If the object is stored inside the foreign object, we have to call the destructor
		if (foreignObject->storage_policy() & JsForeignObject::OBJECT_STORED_CONTIGUOUS)
		{
			foreignObject->type().drop(foreignObject->object());
		}

		// If the foreign object is stored on the heap, we have to destroy it
		if (foreignObject->storage_policy() & JsForeignObject::HEAP_ALLOCATED)
		{
			sge::free(foreignObject);
		}
	}

	void JsForeignObject::init(void* addr, JsValueRef jsObject, const TypeInfo& type, JsValueRef proto, byte storagePolicy)
	{
		// Initialize the foreign object
		auto* foreignObject = new (addr) JsForeignObject{};
		foreignObject->_storagePolicy = storagePolicy;
		foreignObject->_type = &type;

		// Set everything on the javascript object
		JsSetExternalData(jsObject, foreignObject);
		JsSetPrototype(jsObject, proto);
	}

	JsValueRef JsForeignObject::create_js_foreign_object()
	{
		JsValueRef result = JS_INVALID_REFERENCE;
		JsCreateExternalObject(nullptr, &drop_foreign_object, &result);
		return result;
	}

	JsForeignObject* JsForeignObject::from_jsref(JsValueRef object)
	{
		void* foreignObject = nullptr;
		JsGetExternalData(object, &foreignObject);
		return static_cast<JsForeignObject*>(foreignObject);
	}
}

