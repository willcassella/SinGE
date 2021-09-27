// JsForeignProperty.h
#pragma once

#include <ChakraCore.h>

namespace sge
{
    struct PropertyInfo;

    /**
     * \brief Registers a JavaScript property for a foreign type.
     * \param prototype The prototype for the foreign type.
     * \param propertyPrototype The prototype for JS property descriptors.
     * \param getPropId The property id of 'get'.
     * \param setPropId The property id of 'set'.
     * \param prop The property to register.
     * \param name The name of the property.
     */
    void register_foreign_property(
        JsValueRef prototype,
        JsValueRef propertyPrototype,
        JsPropertyIdRef getPropId,
        JsPropertyIdRef setPropId,
        const PropertyInfo& prop,
        const std::string& name);
}
