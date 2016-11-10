// Util.cpp

#include <Core/Reflection/ArgAny.h>
#include <Core/Reflection/Reflection.h>
#include "../private/JsForeignObject.h"
#include "../private/Util.h"

bool sge::to_native_arg(ArgAny* out, JsValueRef value, const TypeInfo& type)
{
	if (&type == &get_type<bool>())
	{
		out->set(to_native_primitive<bool>(value));
		return true;
	}
	else if (&type == &get_type<int8>())
	{
		out->set(to_native_primitive<int8>(value));
		return true;
	}
	else if (&type == &get_type<uint8>())
	{
		out->set(to_native_primitive<uint8>(value));
		return true;
	}
	else if (&type == &get_type<int16>())
	{
		out->set(to_native_primitive<int16>(value));
		return true;
	}
	else if (&type == &get_type<uint16>())
	{
		out->set(to_native_primitive<uint16>(value));
		return true;
	}
	else if (&type == &get_type<int32>())
	{
		out->set(to_native_primitive<int32>(value));
		return true;
	}
	else if (&type == &get_type<uint32>())
	{
		out->set(to_native_primitive<uint32>(value));
		return true;
	}
	else if (&type == &get_type<int64>())
	{
		out->set(to_native_primitive<int64>(value));
		return true;
	}
	else if (&type == &get_type<uint64>())
	{
		out->set(to_native_primitive<uint64>(value));
		return true;
	}
	else if (&type == &get_type<float>())
	{
		out->set(to_native_primitive<float>(value));
		return true;
	}
	else if (&type == &get_type<double>())
	{
		out->set(to_native_primitive<double>(value));
		return true;
	}

	auto* foreignObject = JsForeignObject::from_jsref(value);
	if (foreignObject != nullptr && foreignObject->type() == type)
	{
		out->set(foreignObject->object());
		return true;
	}

	return false;
}

JsValueRef sge::get_or_create_property(JsValueRef object, JsPropertyIdRef prop)
{
	bool hasProperty = false;
	JsHasProperty(object, prop, &hasProperty);

	JsValueRef result;
	if (hasProperty)
	{
		JsGetProperty(object, prop, &result);
	}
	else
	{
		JsCreateObject(&result);
		JsSetProperty(object, prop, result, true);
	}

	return result;
}
