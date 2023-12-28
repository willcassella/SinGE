#pragma once

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <string>

#include "lib/base/reflection/reflection.h"
#include "lib/base/util/interface_utils.h"

namespace sge
{
    struct SGE_BASE_EXPORT IToString
    {
        SGE_REFLECTED_INTERFACE;
        SGE_INTERFACE_1(IToString, to_string)

        std::string (*to_string)(Self self);
    };

    template <typename T>
    struct Impl<IToString, T>
    {
        static std::string to_string(Self self)
        {
            return self.as<T>()->to_string();
        }
    };

    template <>
    struct Impl<IToString, bool>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            if (*self.as<bool>())
            {
                return "true";
            }
            else
            {
                return "false";
            }
        }
    };

    template <>
    struct Impl<IToString, int8_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<int8_t>());
        }
    };

    template <>
    struct Impl<IToString, uint8_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<uint8_t>());
        }
    };

    template <>
    struct Impl<IToString, int16_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<int16_t>());
        }
    };

    template <>
    struct Impl<IToString, uint16_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<uint16_t>());
        }
    };

    template <>
    struct Impl<IToString, int32_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<int32_t>());
        }
    };

    template <>
    struct Impl<IToString, uint32_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<uint32_t>());
        }
    };

    template <>
    struct Impl<IToString, int64_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<int64_t>());
        }
    };

    template <>
    struct Impl<IToString, uint64_t>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<uint64_t>());
        }
    };

    template <>
    struct Impl<IToString, float>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<float>());
        }
    };

    template <>
    struct Impl<IToString, double>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return std::to_string(*self.as<double>());
        }
    };

    template <>
    struct Impl<IToString, char *>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return *self.as<char *>();
        }
    };

    template <>
    struct Impl<IToString, const char *>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return *self.as<const char *>();
        }
    };

    template <size_t Size>
    struct Impl<IToString, char[Size]>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return *self.as<char[Size]>();
        };
    };

    template <size_t Size>
    struct Impl<IToString, const char[Size]>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return *self.as<const char[Size]>();
        }
    };

    template <>
    struct Impl<IToString, std::string>
    {
        static std::string to_string(Self self)
        {
            assert(!self.null());
            return *self.as<std::string>();
        }
    };
}
