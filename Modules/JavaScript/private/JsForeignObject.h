// JsForeignObject.h
#pragma once

#include <ChakraCore.h>
#include <Core/env.h>

namespace sge
{
    struct TypeInfo;

    struct SGE_ALIGNED_BUFFER_HEADER JsForeignObject
    {
        /* Bit flag indicating what this ForeignObject contains, and how it was allocated. */
        using EStoragePolicy = byte;
        enum : EStoragePolicy
        {
            /* This ForeignObject doesn't hold an actual object, but rather a pointer to an object.
             * The destructor for the pointed object will not be called in finalization. */
            SP_OBJECT_POINTER = 0,

            /* This ForeignObject contigiously stores an actual object.
             * The destructor for the stored object will be called in finalization. */
            SP_OBJECT_STORED_CONTIGUOUS = (1 << 0),

            /* This ForeignObject was allocated on the stack, so 'free' will not be called after the destructor. */
            SP_STACK_ALLOCATED = 0,

            /* This ForeignObject was allocated on the heap, so 'free' will be called after the destructor. */
            SP_HEAP_ALLOCATED = (1 << 1)
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        static void stack_init_pointer(
            void* addr,
            JsValueRef jsObject,
            const TypeInfo& type,
            JsValueRef proto,
            void* pointer);

        /**
         * \brief Initializes a JsForeignObject on the heap, with contigous storage for an object.
         * \param jsObject The JsValueRef to set to this object. Must be a JsValueRef previously created by a call to 'create_js_foreign_object'.
         * \param type The type this JsForeignObject is being created for.
         * \param proto The javascript prototype for the type.
         * \return The newly created JsForeignObject. The stored value must be initialized by the user.
         */
        static JsForeignObject* heap_init_object(
            JsValueRef jsObject,
            const TypeInfo& type,
            JsValueRef proto);

        /**
         * \brief Finalizes a JsForeignObject.
         * \param ptr The address of the JsForeignObject.
         */
        static void CALLBACK drop_foreign_object(void* ptr);

    private:

        JsForeignObject() = default;

        static void init(
            void* addr,
            JsValueRef jsObject,
            const TypeInfo& type,
            JsValueRef proto,
            EStoragePolicy storagePolicy);

        /////////////////////
        ///   Functions   ///
    public:

        static JsValueRef create_js_foreign_object();

        static JsForeignObject* from_jsref(JsValueRef object);

        /**
         * \brief Returns the amount of space required to intsantiate a JsForeignObject that holds a pointer.
         * Useful in combination with SGE_STACK_ALLOC.
         */
        static std::size_t object_pointer_alloc_size()
        {
            return sizeof(JsForeignObject) + sizeof(void*);
        }

        ///////////////////
        ///   Methods   ///
    public:

        EStoragePolicy storage_policy() const
        {
            return _storagePolicy;
        }

        const TypeInfo& type() const
        {
            return *_type;
        }

        void* object()
        {
            if (_storagePolicy & SP_OBJECT_STORED_CONTIGUOUS)
            {
                return this + 1;
            }
            else
            {
                return *reinterpret_cast<void**>(this + 1);
            }
        }

        const void* object() const
        {
            if (_storagePolicy & SP_OBJECT_STORED_CONTIGUOUS)
            {
                return this + 1;
            }
            else
            {
                return *reinterpret_cast<const void* const*>(this + 1);
            }
        }

        //////////////////
        ///   Fields   ///
    private:

        EStoragePolicy _storagePolicy;
        const TypeInfo* _type;
    };
}
