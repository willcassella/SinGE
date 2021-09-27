// JsTypeInfo.h
#pragma once

#include <ChakraCore.h>
#include <Core/Reflection/TypeInfo.h>

namespace sge
{
    struct JsTypeInfo final : TypeInfo
    {
        struct Data
        {
            /////////////////////////
            ///   Constructors   ////
        public:

            Data()
                : context(JS_INVALID_REFERENCE), prototype(JS_INVALID_REFERENCE), constructor(JS_INVALID_REFERENCE)
            {
            }

            //////////////////
            ///   Fields   ///
        public:

            JsContextRef context;
            JsValueRef prototype;
            JsValueRef constructor;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        JsTypeInfo(std::string name, Data data)
            : TypeInfo(base_data(std::move(name))), _data(std::move(data))
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        bool has_init() const override
        {
            return true;
        }

        void init(void* addr) const override
        {
            auto* object = static_cast<JsValueRef*>(addr);
            JsSetCurrentContext(_data.context);

            // Create arguments for constructor
            JsCreateObject(object);
            JsValueRef initObj;
            JsCreateObject(&initObj);

            // Call constructor
            run_js_constructor(*object, initObj);

            // Add ref, so Chakra doesn't prematurely collect
            JsAddRef(object, nullptr);
        }

        bool has_copy_init() const override
        {
            return true;
        }

        void copy_init(void* addr, const void* copy) const override
        {
            auto* object = static_cast<JsValueRef*>(addr);
            auto* copyObject = static_cast<const JsValueRef*>(copy);
            JsSetCurrentContext(_data.context);

            // Create arguments for constructor
            JsCreateObject(object);

            // Call constructor
            run_js_constructor(*object, *copyObject);

            // Add ref, so Chakra doesn't prematurely collect
            JsAddRef(object, nullptr);
        }

        bool has_move_init() const override
        {
            return true;
        }

        void move_init(void* addr, void* move) const override
        {
            JsValueRef* selfObj = static_cast<JsValueRef*>(addr);
            JsValueRef* moveObj = static_cast<JsValueRef*>(move);

            *selfObj = *moveObj;
            *moveObj = JS_INVALID_REFERENCE;
        }

        bool has_copy_assign() const override
        {
            return true;
        }

        void copy_assign(void* self, const void* copy) const override
        {
            JsValueRef* selfObj = static_cast<JsValueRef*>(self);
            JsValueRef copyObj = *static_cast<const JsValueRef*>(copy);
            JsSetCurrentContext(_data.context);

            // Run the constructor
            run_js_constructor(selfObj, copyObj);
        }

        bool has_move_assign() const override
        {
            return true;
        }

        void move_assign(void* self, void* move) const override
        {
            move_init(self, move);
        }

        bool has_drop() const override
        {
            return true;
        }

        void drop(void* self) const override
        {
            JsRelease(*static_cast<JsValueRef*>(self), nullptr);
        }

        bool has_equality_compare() const override
        {
            return false;
        }

        bool equality_compare(const void* lhs, const void* rhs) const override
        {
            return false;
        }

        std::size_t num_constructors() const override
        {
            return 0;
        }

        const ConstructorInfo* find_constructor(std::size_t /*argc*/) const override
        {
            return nullptr;
        }

        void enumerate_constructors(FunctionView<ConstructorEnumeratorFn> /*enumerator*/) const override
        {
        }

        std::size_t num_named_constructors() const override
        {
            return 0;
        }

        const ConstructorInfo* find_named_constructor(const char* /*name*/) const override
        {
            return nullptr;
        }

        void enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> /*enuerator*/) const override
        {
        }

        std::size_t num_properties() const override
        {
            return 0;
        }

        const PropertyInfo* find_property(const char* /*name*/) const override
        {
            return nullptr;
        }

        void enumerate_properties(FunctionView<PropertyEnumeratorFn> /*enumerator*/) const override
        {
        }

        std::size_t num_fields() const override
        {
            return 0;
        }

        const FieldInfo* find_field(const char* /*name*/) const override
        {
            return nullptr;
        }

        void enumerate_fields(FunctionView<FieldEnumeratorFn> /*enumerator*/) const override
        {
        }

        const void* get_implementation(const InterfaceInfo& interf) const override
        {
            return nullptr; // TODO
        }

        /**
         * \brief Runs the JavaScript constructor on the given 'self' object, using the given 'init' object.
         * \param self The object to act as the 'this' argument.
         * \param init The object holding the state to initialize with.
         */
        void run_js_constructor(JsValueRef self, JsValueRef init) const
        {
            // Create arguments
            JsValueRef args[] = { self, init };

            // Call constructor
            JsCallFunction(_data.constructor, args, 2, nullptr);
            JsSetPrototype(self, _data.prototype);
        }

    private:

        static TypeInfo::Data base_data(std::string name)
        {
            TypeInfo::Data data{ std::move(name) };
            data.size = sizeof(JsValueRef);
            data.alignment = alignof(JsValueRef);

            return data;
        }

        //////////////////
        ///   Fields   ///
    private:

        Data _data;
    };
}
