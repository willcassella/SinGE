// JsForeignType.h
#pragma once

#include <memory>
#include <vector>
#include <ChakraCore.h>
#include <Core/env.h>
#include "JsForeignConstructor.h"

namespace sge
{
	struct JsForeignType
	{
		////////////////////////
		///   Constructors   ///
	public:

		JsForeignType();
		~JsForeignType();

		static std::unique_ptr<JsForeignType> create(
			JsValueRef propertyProto,
			JsPropertyIdRef getPropId,
			JsPropertyIdRef setPropId,
			const TypeInfo& foreignType);

		///////////////////
		///   Methods   ///
	public:

		JsValueRef js_constructor() const
		{
			return _js_constructor;
		}

		JsValueRef js_prototype() const
		{
			return _js_prototype;
		}

		//////////////////
		///   Fields   ///
	private:

		JsValueRef _js_prototype;
		JsValueRef _js_constructor;
		JsForeignConstructorState _constructor_state;
		std::vector<JsForeignNamedConstructorState> _named_constructors;
	};
}
