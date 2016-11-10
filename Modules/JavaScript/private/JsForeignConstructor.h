// JsForeignConstructor.h
#pragma once

#include <string>
#include <ChakraCore.h>

namespace sge
{
	struct TypeInfo;
	struct ConstructorInfo;

	/* Callback state used for normal constructors. */
	struct JsForeignConstructorState
	{
		/**
		 * \brief The foreign type being constructed.
		 */
		const TypeInfo* foreign_type;

		/**
		 * \brief The JavaScript prototype for the foreign type.
		 */
		JsValueRef js_proto;
	};

	/* Callback state used for named constructors. */
	struct JsForeignNamedConstructorState
	{
		/**
		 * \brief The foreign type being constructed.
		 */
		const TypeInfo* foreign_type;

		/**
		 * \brief The JavaScript prototype for the foreign type.
		 */
		JsValueRef js_proto;

		/**
		 * \brief The consructor used by this NamedConstructor.
		 */
		const ConstructorInfo* foreign_constructor;
	};

	/**
	 * \brief Initializes a JavaScript function wrapping a foreign type constructor.
	 * \param constructorState The state object for the JavaScript function.
	 * \param foreignType The foreign type to construct.
	 * \param jsPrototype The JavaScript prototype for the type being constructed.
	 * \return A JavaScript reference to the function object.
	 */
	JsValueRef init_foreign_constructor(
		JsForeignConstructorState& constructorState,
		const TypeInfo& foreignType,
		JsValueRef jsPrototype);

	/**
	 * \brief Initializes a JavaScript function wrapping a foreign type named constructor.
	 * \param constructorState The state object for the JavaScript function.
	 * \param foreignType The foreign type to construct.
	 * \param jsPrototype The JavaScript prototype for the foreign type.
	 * \param jsConstructor A JavaScript reference to the foreign object's constructor.
	 * \param foreignConstructor The named constructor to wrap.
	 * \param name The name of the named constructor.
	 */
	void init_foreign_named_constructor(
		JsForeignNamedConstructorState& constructorState,
		const TypeInfo& foreignType,
		JsValueRef jsPrototype,
		JsValueRef jsConstructor,
		const ConstructorInfo& foreignConstructor,
		const std::string& name);
}
