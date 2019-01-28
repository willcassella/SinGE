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
        {
            std::memset(_value, 0, sizeof(_value));
        }
        FixedString(const char* str)
            : FixedString()
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
            std::strncpy(_value, str, Size);
        }

        /**
         * \brief Compares for equality only up to the end of this string.
         * \param str The string to compare.
         * \return Whether this string is equal to the given string up to the length of this string.
         */
        bool eq_up_to_size(const char* str)
        {
            return std::strncmp(_value, str, Size) == 0;
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
