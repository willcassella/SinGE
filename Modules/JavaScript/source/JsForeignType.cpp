// JsForeignType.cpp

#include <Core/Reflection/TypeInfo.h>
#include "../private/JsForeignType.h"
#include "../private/JsForeignProperty.h"

namespace sge
{
	JsForeignType::JsForeignType()
		: _js_prototype(JS_INVALID_REFERENCE),
		_js_constructor(JS_INVALID_REFERENCE)
	{
	}

	JsForeignType::~JsForeignType()
	{
		// Do stuf...
	}

	std::unique_ptr<JsForeignType> JsForeignType::create(
		JsValueRef propertyProto,
		JsPropertyIdRef getPropId,
		JsPropertyIdRef setPropId,
		const TypeInfo& foreignType)
	{
		// Create the type state object
		auto result = std::make_unique<JsForeignType>();
		JsCreateObject(&result->_js_prototype);
		JsAddRef(result->_js_prototype, nullptr);

		// Intitialize the constructor state object
		result->_js_constructor = init_foreign_constructor(result->_constructor_state, foreignType, result->_js_prototype);

		// Allocate named constructor state objects
		result->_named_constructors.reserve(foreignType.num_named_constructors());

		// Create named constructors
		foreignType.enumerate_named_constructors([&](const std::string& name, const ConstructorInfo& ctor)
		{
			// Initialize the named constructor
			result->_named_constructors.push_back({});
			init_foreign_named_constructor(
				result->_named_constructors.back(),
				foreignType,
				result->js_prototype(),
				result->js_constructor(),
				ctor,
				name);
		});

		// Add all properties to the prototype
		foreignType.enumerate_properties([&](const std::string& name, const PropertyInfo& prop)
		{
			register_foreign_property(result->_js_prototype, propertyProto, getPropId, setPropId, prop, name);
		});

		return result;
	}
}
