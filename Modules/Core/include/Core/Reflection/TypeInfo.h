// TypeInfo.h
#pragma once

#include <cstdint>
#include <string>
#include <typeinfo>
#include "../Functional/FunctionView.h"
#include "../config.h"

namespace sge
{
    struct InterfaceInfo;
    struct ConstructorInfo;
    struct PropertyInfo;
    struct FieldInfo;

    using TypeFlags_t = uint32;
    enum TypeFlags : TypeFlags_t
    {
        /**
         * \brief This type has no special flags.
         */
        TF_NONE = 0,

        /**
         * \brief When recursing through type-property definitions, this type should terminate recursion.
         * TODO: Should there be a more context-sensitive solution to this?
         */
        TF_RECURSE_TERMINAL = (1 << 0),

        /**
         * \brief This type is a native (C++) type.
         */
        TF_NATIVE = (1 << 1),

        /**
         * \brief This type is a native (C++) primitive type.
         */
        TF_PRIMITIVE = (1 << 2) | TF_NATIVE | TF_RECURSE_TERMINAL,

        /**
         * \brief This type may not be constructed by scripts.
         */
        TF_SCRIPT_NOCONSTRUCT = (1 << 3) | TF_NATIVE,

        /**
         * \brief This type represents a container.
         */
        TF_CONTAINER = (1 << 4),
    };

    struct SGE_CORE_API TypeInfo
    {
        using ConstructorEnumeratorFn = void(std::size_t argc, const ConstructorInfo& ctor);
        using NamedConstructorEnumeratorFn = void(const char* name, const ConstructorInfo& ctor);
        using PropertyEnumeratorFn = void(const char* name, const PropertyInfo& prop);
        using FieldEnumeratorFn = void(const char* name, const FieldInfo& field);

        struct Data
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            Data(std::string name)
                : flags(TF_NONE),
                name(std::move(name)),
                size(0),
                alignment(0),
                base(nullptr),
                native_type_info(nullptr),
                is_empty(false)
            {
            }

            //////////////////
            ///   Fields   ///
        public:

            TypeFlags_t flags;
            std::string name;
            std::size_t size;
            std::size_t alignment;
            const TypeInfo* base;
            const std::type_info* native_type_info;
            bool is_empty;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        explicit TypeInfo(Data data)
            : _data(std::move(data))
        {
            // Generate hash code
            if (_data.native_type_info != nullptr)
            {
                _hash_code = _data.native_type_info->hash_code();
            }
            else
            {
                _hash_code = std::hash<std::string>{}(_data.name);
            }
        }

        ///////////////////
        ///   Methods   ///
    public:

        /**
         * \brief Returns the flags associated with this type.
         */
        TypeFlags_t flags() const
        {
            return _data.flags;
        }

        /**
         * \brief Returns the name of this type.
         */
        const std::string& name() const
        {
            return _data.name;
        }

        /**
         * \brief Returns the size (in bytes) of this type.
         */
        std::size_t size() const
        {
            return _data.size;
        }

        /**
         * \brief Pads the size of this type to a multiple of the given alignment.
         * \param alignment The alignment to pad to.
         * \return The aligned size of this type.
         */
        std::size_t aligned_size(const std::size_t alignment = alignof(std::max_align_t)) const
        {
            const auto size = _data.size;

            if (alignment == 0)
            {
                return size;
            }

            const auto remainder = size % alignment;

            if (remainder == 0)
            {
                return size;
            }
            else
            {
                return size + alignment - remainder;
            }
        }

        /**
         * \brief Returns the alignment requirements of this type.
         */
        std::size_t alignment() const
        {
            return _data.alignment;
        }

        /**
         * \brief Returns a hash code for this type.
         */
        std::size_t hash_code() const
        {
            return _hash_code;
        }

        /**
         * \brief Returns a pointer to the base type (if applicable) of this type.
         */
        const TypeInfo* base() const
        {
            return _data.base;
        }

        /**
         * \brief Returns whether this type occupies any space.
         */
        bool is_empty() const
        {
            return _data.is_empty;
        }

        /**
         * \brief Returns whether this type is a native (C++) type.
         */
        bool is_native() const
        {
            return (_data.flags & TF_NATIVE) != 0;
        }

        /**
         * \brief Returns whether this type is a native (C++) primitive type.
         */
        bool is_primitive() const
        {
            return (_data.flags & TF_PRIMITIVE) != 0;
        }

        /**
         * \brief Returns whether this type supports default-initializiation.
         */
        virtual bool has_init() const = 0;

        /**
         * \brief Initializes an instance of this type.
         * \param addr The address to initialize the instance at.
         */
        virtual void init(void* addr) const = 0;

        /**
         * \brief Returns whether this type supports copy-initialization.
         */
        virtual bool has_copy_init() const = 0;

        /**
         * \brief Copy-initializes an instance of this type.
         * \param addr The address to initialize the instance at.
         * \param copy The instance to copy-initialize from.
         */
        virtual void copy_init(void* addr, const void* copy) const = 0;

        /**
         * \brief Returns whether this type supports move-initialization.
         */
        virtual bool has_move_init() const = 0;

        /**
         * \brief Move-initializes an instance of this type.
         * \param addr The address to initialize the instance at.
         * \param move The instance to move-initialize from.
         */
        virtual void move_init(void* addr, void* move) const = 0;

        /**
         * \brief Returns whether this type supports copy-assignment.
         */
        virtual bool has_copy_assign() const = 0;

        /**
         * \brief Copy-assigns an instance of this type.
         * \param self The instance to copy-assign to.
         * \param copy The instance to copy-assign from.
         */
        virtual void copy_assign(void* self, const void* copy) const = 0;

        /**
         * \brief Returns whether this type supports move-assignment.
         */
        virtual bool has_move_assign() const = 0;

        /**
         * \brief Move-assigns an instance of this type.
         * \param self The instance to move-assign to.
         * \param move The instance to move-assign from.
         */
        virtual void move_assign(void* self, void* move) const = 0;

        /**
         * \brief Returns whether this type supports dropping (destruction).
         */
        virtual bool has_drop() const = 0;

        /**
         * \brief Drops (destroys) an instance of this type.
         * \param self The instance to drop.
         */
        virtual void drop(void* self) const = 0;

        /**
         * \brief Returns whether equality comparison is supported by this type.
         */
        virtual bool has_equality_compare() const = 0;

        /**
         * \brief Perofms an equality comparison between two instances of this type.
         * \return The result of the comparison.
         */
        virtual bool equality_compare(const void* lhs, const void* rhs) const = 0;

        /**
        * \brief Returns the number of constructors for this type. This will match the number of calls to 'enumerator'
        * within 'enumerate_constructors'.
        */
        virtual std::size_t num_constructors() const = 0;

        /**
         * \brief Searches for an unnamed constructor with the given number of arguments on this type.
         * \param argc The arity of the constructor to search for.
         * \return A pointer to the information for the constructor if found, 'nullptr' if not.
         */
        virtual const ConstructorInfo* find_constructor(std::size_t argc) const = 0;

        /**
         * \brief Enumerates all unnamed constructors on this type.
         * \param enumerator A function to call with the arity and information for the constructor at each iteration.
         */
        virtual void enumerate_constructors(FunctionView<ConstructorEnumeratorFn> enumerator) const = 0;

        /**
        * \brief Returns the number of named constructors for this type. This will match the number of calls to 'enumerator'
        * within 'enumerate_constructors'.
        */
        virtual std::size_t num_named_constructors() const = 0;

        /**
         * \brief Searches for a named constructor with the given name on this type.
         * \param name The name of the constructor to search for.
         * \return A pointer to the information for the constructor if found, 'nullptr' otherwise.
         */
        virtual const ConstructorInfo* find_named_constructor(const char* name) const = 0;

        /**
         * \brief Enumerates all named constructors of this type.
         * \param enumerator A function to call with the name and information for the constructor at each iteration.
         */
        virtual void enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> enumerator) const = 0;

        /**
         * \brief Returns the number of properties on this type. This will match the number of calls to 'enumerator'
         * within 'enumerate_properties'.
         */
        virtual std::size_t num_properties() const = 0;

        /**
         * \brief Searches for a property with the given name on this type.
         * \param name The name of the property to search for.
         * \return A pointer to the information for the property if found, 'nullptr' otherwise.
         */
        virtual const PropertyInfo* find_property(const char* name) const = 0;

        /**
         * \brief Enumerates all properties of this type.
         * \param enumerator A function to call with the name and information for the property at each iteration.
         */
        virtual void enumerate_properties(FunctionView<PropertyEnumeratorFn> enumerator) const = 0;

        /**
         * \brief Returns the number of fields on this type. This will match the number of calls to 'enumerator'
         * within 'enumerate_fields'.
         */
        virtual std::size_t num_fields() const = 0;

        /**
         * \brief Searches for a field with the given name on this type.
         * \param name The name of the field to search for.
         * \return A pointer to the information for the field if found, 'nullptr' otherwise.
         */
        virtual const FieldInfo* find_field(const char* name) const = 0;

        /**
         * \brief Enumerates all fields of this type.
         * \param enumerator A function to call with the name and information of the field at each iteration.
         */
        virtual void enumerate_fields(FunctionView<FieldEnumeratorFn> enumerator) const = 0;

        /**
         * \brief Returns a pointer to the vtable object created for this type for the specified interface.
         * In general you should use the global 'get_vtable' function, as it will search implementations on the type, as well as on the interface.
         * \param interf The interface to find the vtable for.
         * \return A pointer to the vtable object, which may be null if this type has not implemented the interface.
         */
        virtual const void* get_implementation(const InterfaceInfo& interf) const = 0;

        /////////////////////
        ///   Operators   ///
    public:

        friend bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
        {
            return &lhs == &rhs ||
                (lhs._data.native_type_info && rhs._data.native_type_info && *lhs._data.native_type_info == *rhs._data.native_type_info);
        }
        friend bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
        {
            return !(lhs == rhs);
        }

        //////////////////
        ///   Fields   ///
    private:

        Data _data;
        std::size_t _hash_code;
    };
}
