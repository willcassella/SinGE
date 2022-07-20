// Reflection.h
#pragma once

#include "base/config.h"
#include "base/stde/TypeTraits.h"
#include "base/reflection/NativeTypeInfo.h"
#include "base/reflection/InterfaceInfo.h"
#include "base/reflection/EnumTypeInfo.h"

namespace sge
{
    namespace specialized
    {
        /* Generic implementation of 'GetType' */
        template <typename T>
        struct GetType
        {
            static const auto &get_type()
            {
                return std::decay_t<T>::type_info;
            }

            static const auto &get_type(const T &value)
            {
                return value.get_type();
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<void>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<bool>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<int8>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<uint8>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<int16>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<uint16>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<int32>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<uint32>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<int64>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<uint64>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<float>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<double> final
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type(...)
            {
                return type_info;
            }
        };

        template <>
        struct SGE_BASE_EXPORT GetType<std::string>
        {
            static const NativeTypeInfo type_info;

            static const NativeTypeInfo &get_type()
            {
                return type_info;
            }

            static const NativeTypeInfo &get_type(const std::string & /*value*/)
            {
                return type_info;
            }
        };
    }

    template <class I, typename T>
    struct Impl;

    /////////////////////
    ///   Functions   ///

    template <typename I>
    const InterfaceInfo &get_interface()
    {
        return I::interface_info;
    }

    template <typename T>
    const auto &get_type()
    {
        return specialized::GetType<T>::get_type();
    }

    template <typename T>
    const auto &get_type(const T &value)
    {
        return specialized::GetType<T>::get_type(value);
    }

    template <typename I>
    const I *get_vtable(const TypeInfo &type)
    {
        const auto &interf = get_interface<I>();

        // See if this type has defined an implementation for this interface
        const I *impl = static_cast<const I *>(type.get_implementation(interf));

        if (impl)
        {
            return impl;
        }

        // See if this interface has defined an implementation for this class
        impl = static_cast<const I *>(interf.get_implementation(type));
        return impl;
    }
}

//////////////////
///   Macros   ///

/* Use this macro inside the definition of a type in order for it to be recognized by the reflection system. */
#define SGE_REFLECTED_TYPE                        \
    static const ::sge::NativeTypeInfo type_info; \
    const ::sge::NativeTypeInfo &get_type() const { return type_info; }

/* Use this macro inside the header for a type in order for it to be recognized by the reflection system. */
#define SGE_REFLECTED_ENUM(API, E)                                             \
    namespace sge                                                              \
    {                                                                          \
        namespace specialized                                                  \
        {                                                                      \
            template <>                                                        \
            struct API GetType<E>                                              \
            {                                                                  \
                static const EnumTypeInfo type_info;                           \
                static const EnumTypeInfo &get_type(...) { return type_info; } \
            };                                                                 \
        }                                                                      \
    }

/* Use this macro in the definition of an interface, in order for it to be recorgnized by the reflection system. */
#define SGE_REFLECTED_INTERFACE static const ::sge::InterfaceInfo interface_info;

/* Internal macro used by SGE_INTERFACE macros. */
#define SGE_INTERFACE_0(INTERF, ...)       \
    template <typename T>                  \
    static const INTERF &get_impl()        \
    {                                      \
        using ImplT = Impl<INTERF, T>;     \
        static INTERF vtable{__VA_ARGS__}; \
        return vtable;                     \
    }

#define SGE_EXPAND(x) x
#define SGE_INTERFACE_MEMBER(M) ImplT::M
#define SGE_INTERFACE_RECURSE_END(INTERF, m, ...) SGE_INTERFACE_0(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m))

/* Use these macros in the definition of an interface, where 'INTERF' is name of the interface type, and the proceeding arguments are
 * the names of the interface members to be automatically pulled from the 'Impl' specialization.
 * Use 'SGE_INTERFACE_X' where 'X' is the number of interface member arguments given. */
#define SGE_INTERFACE_1(INTERF, m) SGE_INTERFACE_0(INTERF, SGE_INTERFACE_MEMBER(m))
#define SGE_INTERFACE_2(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_RECURSE_END(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_3(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_2(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_4(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_3(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_5(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_4(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_6(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_5(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_7(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_6(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
#define SGE_INTERFACE_8(INTERF, m, ...) SGE_EXPAND(SGE_INTERFACE_7(INTERF, __VA_ARGS__, SGE_INTERFACE_MEMBER(m)))
