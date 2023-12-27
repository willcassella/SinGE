#pragma once

#include <cassert>
#include <string>

#include "lib/base/reflection/reflection.h"
#include "lib/base/util/interface_utils.h"

namespace sge
{
    struct SGE_BASE_EXPORT IFromString
    {
        SGE_REFLECTED_INTERFACE;
        SGE_INTERFACE_1(IFromString, from_string)

        std::size_t (*from_string)(SelfMut self, const char *str, std::size_t len);
    };

    template <typename T>
    struct Impl<IFromString, T>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t len)
        {
            return self.as<T>()->from_string(str, len);
        }
    };

    template <>
    struct Impl<IFromString, int32>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<int32>() = std::strtol(str, &end, 10);
            return end - str;
        }
    };

    template <>
    struct Impl<IFromString, uint32>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<uint32>() = std::strtoul(str, &end, 10);
            return end - str;
        }
    };

    template <>
    struct Impl<IFromString, int64>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<int64>() = std::strtoll(str, &end, 10);
            return end - str;
        }
    };

    template <>
    struct Impl<IFromString, uint64>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<uint64>() = std::strtoull(str, &end, 10);
            return end - str;
        }
    };

    template <>
    struct Impl<IFromString, float>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<float>() = std::strtof(str, &end);
            return end - str;
        }
    };

    template <>
    struct Impl<IFromString, double>
    {
        static std::size_t from_string(SelfMut self, const char *str, std::size_t /*len*/)
        {
            assert(!self.null());
            char *end;
            *self.as<double>() = std::strtod(str, &end);
            return str - end;
        }
    };

    template <typename T>
    std::size_t from_string(T &self, const char *str, std::size_t len)
    {
        return Impl<IFromString, T>::from_string(&self, str, len);
    }
}
