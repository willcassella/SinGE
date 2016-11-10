// JsForeignObject.h
#pragma once

#include <ChakraCore.h>

namespace sge
{
	struct TypeInfo;
	struct ConstructorInfo;

	struct JsForeignObject
	{
		/////////////////////
		///   Functions   ///
	public:

		/* Initializes a JsForeignObject with the given construct and arguments. */
		static JsForeignObject* constructor_init(
			JsValueRef* out,
			const TypeInfo& type,
			JsValueRef proto,
			const ConstructorInfo& constructor,
			JsValueRef* args);

		/* Default-initializes a JsForeignObject of the given type. */
		static JsForeignObject* default_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto);

		/* Copy-initializes a JsForeignObject of the given type. */
		static JsForeignObject* copy_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto, const void* copy);

		/* Move-initializes a JsForeignObject of the given type. */
		static JsForeignObject* move_init(JsValueRef* out, const TypeInfo& type, JsValueRef proto, void* move);

		/* Extracts a JsForeignObject from a JS object. */
		static JsForeignObject* from_jsref(JsValueRef object);

	private:

		JsForeignObject() = default;

		static JsForeignObject* create(JsValueRef* out, const TypeInfo& type, JsValueRef proto);

		///////////////////
		///   Methods   ///
	public:

		void* object()
		{
			return this + 1;
		}

		const void* object() const
		{
			return this + 1;
		}

		const TypeInfo& type() const
		{
			return *_type;
		}

		//////////////////
		///   Fields   ///
	private:

		const TypeInfo* _type;
	};
}
