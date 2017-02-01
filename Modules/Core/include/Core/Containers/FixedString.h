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

        /////////////////////
        ///   Operators   ///
    public:

        friend bool operator==(const FixedString& lhs, const char* rhs)
        {
            return std::strcmp(lhs.c_str(), rhs) == 0;
        }
        friend bool operator==(const char* lhs, const FixedString& rhs)
        {
            return std::strcmp(lhs, rhs.c_str()) == 0;
        }
        friend bool operator!=(const FixedString& lhs, const char* rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator!=(const char* lhs, const FixedString& rhs)
        {
            return !(lhs == rhs);
        }

        template <std::size_t RhsSize>
        friend bool operator==(const FixedString& lhs, const FixedString<RhsSize>& rhs)
        {
            return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
        }

        template <std::size_t RhsSize>
        friend bool operator!=(const FixedString& lhs, const FixedString<RhsSize>& rhs)
        {
            return !(lhs == rhs);
        }

        //////////////////
        ///   Fields   ///
    private:

        char _value[Size + 1];
    };
}
