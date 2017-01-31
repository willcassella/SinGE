// FixedString.h
#pragma once

#include <cstring>

namespace sge
{
    template <std::size_t Size>
    struct FixedString
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        FixedString()
            : _value{ 0 }
        {
        }
        FixedString(const char* str)
        {
            set_str(str);
        }

        ///////////////////
        ///   Methods   ///
    public:

        const char* c_str() const
        {
            return _value;
        }

        void set_str(const char* str)
        {
            std::memset(_value, 0, sizeof(_value));
            std::strncpy(_value, str, Size);
        }

        //////////////////
        ///   Fields   ///
    private:

        char _value[Size + 1];
    };
}
