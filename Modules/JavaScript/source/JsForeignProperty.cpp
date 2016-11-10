// JsForeignProperty.cpp

#include <Core/Reflection/Reflection.h>
#include "../private/JsForeignObject.h"
#include "../private/JsForeignProperty.h"
#include "../private/Util.h"

namespace sge
{
	template <typename T>
	JsValueRef CALLBACK js_primitive_getter(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short /*argc*/,
		void* callbackState)
	{
		// Get the 'this' argument as a foreign object
		auto* object = JsForeignObject::from_jsref(args[0]);

		// Get the information for the property
		auto* prop = static_cast<const PropertyInfo*>(callbackState);

		// Run the getter
		JsValueRef ret = JS_INVALID_REFERENCE;
		prop->get(object->object(), nullptr, [&](Any<> out) {
			ret = from_native_primitive<T>(out);
		});

		return ret;
	}

	template <typename T>
	JsValueRef CALLBACK js_primitive_setter(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* callbackState)
	{
		// Make sure we have the correct number of arguments
		if (argc < 2)
		{
			return JS_INVALID_REFERENCE;
		}

		// Get the 'this' argument as a foreign object
		auto* object = JsForeignObject::from_jsref(args[0]);

		// Get the 'value' argument as a primitive
		auto value = to_native_primitive<T>(args[1]);

		// Get the information for the property
		auto* prop = static_cast<const PropertyInfo*>(callbackState);

		// Run the setter
		prop->set(object->object(), nullptr, &value);

		return JS_INVALID_REFERENCE;
	}

	template <typename T>
	void create_foreign_primitive_property(
		JsValueRef propertyDescriptor,
		JsPropertyIdRef getPropId,
		JsPropertyIdRef setPropId,
		const PropertyInfo& prop)
	{
		// Create and set the getter
		JsValueRef getter = JS_INVALID_REFERENCE;
		JsCreateFunction(&js_primitive_getter<T>, const_cast<PropertyInfo*>(&prop), &getter);
		JsSetProperty(propertyDescriptor, getPropId, getter, true);

		if (prop.is_read_only())
		{
			return;
		}

		// Create and set the setter
		JsValueRef setter = JS_INVALID_REFERENCE;
		JsCreateFunction(&js_primitive_setter<T>, const_cast<PropertyInfo*>(&prop), &setter);
		JsSetProperty(propertyDescriptor, setPropId, setter, true);
	}

	JsValueRef CALLBACK js_getter_wrapper(
		JsValueRef callee,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* callbackState)
	{
		// Get the 'this' argument as a foreign object
		auto* self = JsForeignObject::from_jsref(args[0]);

		// Get the property we're getting
		auto* prop = static_cast<const PropertyInfo*>(callbackState);

		// Run the getter for the property
		JsValueRef ret = nullptr;
		prop->get(self->object(), nullptr, [&](Any<> value) {
			//ret = from_native(value);
		});

		return ret;
	}

	JsValueRef CALLBACK js_setter_wrapper(
		JsValueRef /*callee*/,
		bool /*isConstructCall*/,
		JsValueRef* args,
		unsigned short argc,
		void* callbackState)
	{
		// Get the 'this' argument as a foreign object
		auto* object = JsForeignObject::from_jsref(args[0]);

		// Get the property we're getting
		auto* prop = static_cast<const PropertyInfo*>(callbackState);

		return nullptr;
	}

	void create_foreign_property(
		JsValueRef propertyDescriptor,
		JsPropertyIdRef getPropId,
		JsPropertyIdRef setPropId,
		const PropertyInfo& prop)
	{
	}

	void register_foreign_property(
		JsValueRef prototype,
		JsValueRef propertyPrototype,
		JsPropertyIdRef getPropId,
		JsPropertyIdRef setPropId,
		const PropertyInfo& prop,
		const std::string& name)
	{
		// Create the propertyDescriptor
		JsValueRef propertyDescriptor = JS_INVALID_REFERENCE;
		JsCreateObject(&propertyDescriptor);
		JsSetPrototype(propertyDescriptor, propertyPrototype);

		if (&prop.type() == &get_type<bool>())
		{
			create_foreign_primitive_property<bool>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<int8>())
		{
			create_foreign_primitive_property<int8>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<uint8>())
		{
			create_foreign_primitive_property<uint8>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<int16>())
		{
			create_foreign_primitive_property<int16>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<uint16>())
		{
			create_foreign_primitive_property<uint16>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<int32>())
		{
			create_foreign_primitive_property<int32>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<uint32>())
		{
			create_foreign_primitive_property<uint32>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<int64>())
		{
			create_foreign_primitive_property<int64>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<uint64>())
		{
			create_foreign_primitive_property<uint64>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<float>())
		{
			create_foreign_primitive_property<float>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else if (&prop.type() == &get_type<double>())
		{
			create_foreign_primitive_property<double>(propertyDescriptor, getPropId, setPropId, prop);
		}
		else
		{
			create_foreign_property(propertyDescriptor, getPropId, setPropId, prop);
		}

		// Set the property
		bool success = false;
		JsPropertyIdRef propId = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8(name.c_str(), &propId);
		JsDefineProperty(prototype, propId, propertyDescriptor, &success);
	}
}
