// Util.h
#pragma once

#include <string>
#include <ChakraCore.h>
#include <Core/Reflection/Any.h>

namespace sge
{
	template <typename T>
	auto to_native_primitive(JsValueRef value) -> std::enable_if_t<std::is_integral<T>::value, T>
	{
		int i;
		JsNumberToInt(value, &i);
		return static_cast<T>(i);
	}

	template <typename T>
	auto to_native_primitive(JsValueRef value) -> std::enable_if_t<std::is_floating_point<T>::value, T>
	{
		double d;
		JsNumberToDouble(value, &d);
		return static_cast<T>(d);
	}

	template <>
	inline bool to_native_primitive<bool>(JsValueRef value)
	{
		bool b;
		JsBooleanToBool(value, &b);
		return b;
	}

	template <typename T>
	auto from_native_primitive(Any<> any) -> std::enable_if_t<std::is_integral<T>::value, JsValueRef>
	{
		T value = *static_cast<const T*>(any.object());

		JsValueRef ret;
		JsIntToNumber(static_cast<int>(value), &ret);
		return ret;
	}

	template <typename T>
	auto from_native_primitive(Any<> any) -> std::enable_if_t<std::is_floating_point<T>::value, JsValueRef>
	{
		T value = *static_cast<const T*>(any.object());

		JsValueRef ret;
		JsDoubleToNumber(static_cast<double>(value), &ret);
		return ret;
	}

	template <>
	inline JsValueRef from_native_primitive<bool>(Any<> any)
	{
		bool value = *static_cast<const bool*>(any.object());

		JsValueRef ret;
		JsBoolToBoolean(value, &ret);
		return ret;
	}

	bool to_native_arg(ArgAny* out, JsValueRef value, const TypeInfo& type);

	JsValueRef get_or_create_property(JsValueRef object, JsPropertyIdRef prop);
}
