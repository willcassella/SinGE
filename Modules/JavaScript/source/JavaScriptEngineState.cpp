// JavaScriptEngineState.cpp

#include <fstream>
#include <iostream>
#include "../private/JavaScriptEngineState.h"
#include "../private/Util.h"
#include "../private/JsTypeInfo.h"
#include "../private/JsForeignType.h"

namespace sge
{
	void JavaScriptEngine::State::load_script(const char* path)
	{
		std::wifstream file(path);

		// If the file couldn't be opened, we can't do anything
		if (!file)
		{
			return;
		}

		// Determine the size of the file
		file.seekg(0, std::ios::end);
		auto length = file.tellg();
		file.seekg(0, std::ios::beg);

		// Load all contents into memory
		std::vector<wchar_t> contents(length);
		file.read(&contents.front(), length);
		file.close();

		// Run the script, checking for error
		auto error = JsRunScript(&contents.front(), js_runtime.source_context++, L"", nullptr);

		// Handle exception error from script
		if (error == JsErrorScriptException)
		{
			// Get the exception
			JsValueRef exception = JS_INVALID_REFERENCE;
			JsGetAndClearException(&exception);

			// Get the exception as a string
			JsValueRef exceptionString = JS_INVALID_REFERENCE;
			JsConvertValueToString(exception, &exceptionString);
			const wchar_t* str = nullptr;
			std::size_t len = 0;
			JsStringToPointer(exceptionString, &str, &len);

			// Print it
			std::wcout << L"Exception thrown: ";
			std::wcout.write(str, len);
			std::wcout << std::endl;
		}
	}

	void JavaScriptEngine::State::register_foreign_type(const TypeInfo& type)
	{
		JsSetCurrentContext(js_runtime.context);

		auto foreignType = JsForeignType::create(
			js_property.property_prototype,
			js_property.get_prop_id,
			js_property.set_prop_id,
			type);

		// Generate a name for the type, and put it in the proper scope
		JsValueRef parent = js_environment.global_object;
		std::size_t nameStart = 0;
		for (std::size_t i = 0; i < type.name().size(); ++i)
		{
			if (type.name()[i] == ':')
			{
				JsPropertyIdRef scopeName;
				JsGetPropertyIdFromNameUtf8(type.name().substr(nameStart, i).c_str(), &scopeName);
				parent = get_or_create_property(parent, scopeName);

				i += 1;
				nameStart = i + 1;
			}
		}

		// Register the constructor function as a global
		JsPropertyIdRef namePropId = JS_INVALID_REFERENCE;
		JsGetPropertyIdFromNameUtf8(type.name().c_str() + nameStart, &namePropId);
		JsSetProperty(parent, namePropId, foreignType->js_constructor(), true);

		// Add the type's prototype to the prototype map
		js_prototypes[&type] = foreignType->js_prototype();

		// Add property to set of registerd types
		registered_foreign_types[&type] = std::move(foreignType);
	}

	void JavaScriptEngine::State::register_js_type(const char* name, JsValueRef constructor, JsValueRef proto)
	{
		// Create the type data
		JsTypeInfo::Data type_data;
		type_data.context = js_runtime.context;
		type_data.constructor = constructor;
		type_data.prototype = proto;

		// Create the TypeInfo object
		auto typeInfo = std::make_unique<JsTypeInfo>(name, std::move(type_data));

		// Add the type's prototype to the prototype map
		js_prototypes[typeInfo.get()] = proto;

		// Add it to the list of types
		js_types.insert(std::make_pair(name, std::move(typeInfo)));
	}

	JsValueRef JavaScriptEngine::State::get_js_prototype(const TypeInfo& type)
	{
		// TODO: Error handling
		return js_prototypes.find(&type)->second;
	}
}
