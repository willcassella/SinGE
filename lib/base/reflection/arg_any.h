#pragma once

#include <stdint.h>

#include "lib/base/env.h"

namespace sge
{
    struct ArgAny
    {
        union Storage
        {
            bool b;
            int8_t i8;
            uint8_t u8;
            int16_t i16;
            uint16_t u16;
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            uint64_t u64;
            float f;
            double d;
            const void *obj;
        };

        template <typename T>
        const T &get() const
        {
            return *static_cast<const T *>(_value.obj);
        }

        template <typename T>
        void set(const T &obj)
        {
            _value.obj = &obj;
        }

        template <typename T>
        void set(const T &&obj) = delete;

        template <typename T>
        void set(T *ptr)
        {
            _value.obj = ptr;
        }

        template <typename T>
        void set(const T *ptr)
        {
            _value.obj = ptr;
        }

        void set(bool value)
        {
            _value.b = value;
        }

        void set(int8_t value)
        {
            _value.i8 = value;
        }

        void set(uint8_t value)
        {
            _value.u8 = value;
        }

        void set(int16_t value)
        {
            _value.i16 = value;
        }

        void set(uint16_t value)
        {
            _value.u16 = value;
        }

        void set(int32_t value)
        {
            _value.i32 = value;
        }

        void set(uint32_t value)
        {
            _value.u32 = value;
        }

        void set(int64_t value)
        {
            _value.i64 = value;
        }

        void set(uint64_t value)
        {
            _value.u64 = value;
        }

        void set(float value)
        {
            _value.f = value;
        }

        void set(double value)
        {
            _value.d = value;
        }

    private:
        Storage _value;
    };

    template <>
    inline const bool &ArgAny::get() const
    {
        return _value.b;
    }

    template <>
    inline const int8_t &ArgAny::get() const
    {
        return _value.i8;
    }

    template <>
    inline const uint8_t &ArgAny::get() const
    {
        return _value.u8;
    }

    template <>
    inline const int16_t &ArgAny::get() const
    {
        return _value.i16;
    }

    template <>
    inline const uint16_t &ArgAny::get() const
    {
        return _value.u16;
    }

    template <>
    inline const int32_t &ArgAny::get() const
    {
        return _value.i32;
    }

    template <>
    inline const uint32_t &ArgAny::get() const
    {
        return _value.u32;
    }

    template <>
    inline const int64_t &ArgAny::get() const
    {
        return _value.i64;
    }

    template <>
    inline const uint64_t &ArgAny::get() const
    {
        return _value.u64;
    }

    template <>
    inline const float &ArgAny::get() const
    {
        return _value.f;
    }

    template <>
    inline const double &ArgAny::get() const
    {
        return _value.d;
    }
}
