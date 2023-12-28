#pragma once

#include "lib/base/interfaces/to_string.h"

namespace sge
{
    inline bool string_ends_with(const std::string &str, const std::string &end)
    {
        if (str.length() < end.length())
        {
            return false;
        }

        return str.compare(str.length() - end.length(), end.length(), end) == 0;
    }

    template <typename T>
    std::string to_string(const T &value)
    {
        return Impl<IToString, T>::to_string(&value);
    }

    inline void format_detail(std::string &out, const char *fmt, const std::string *values, size_t numValues)
    {
        // Replace all instances of '@' in the format string with the given values.
        for (; *fmt != 0 && numValues > 0; ++fmt)
        {
            if (*fmt == '@')
            {
                out.append(*values);
                ++values;
                --numValues;
            }
            else
            {
                out += *fmt;
            }
        }

        // Append the rest of the format string to the output string
        out.append(fmt);
    }

    template <typename... Ts>
    std::string format(const char *fmt, const Ts &...ts)
    {
        // Create an array of strings to put into the result string
        const std::string values[] = {to_string(ts)...};

        // Calculate total size of value strings
        size_t buffSize = 0;
        for (const auto &str : values)
        {
            buffSize += str.size();
        }

        // Create an output string, and reserve space to minimize dynamic reallocation
        std::string result;
        result.reserve(buffSize + strlen(fmt));

        // Format the string
        format_detail(result, fmt, values, sizeof...(Ts));
        return result;
    }

    template <typename... Ts>
    std::string format(const std::string &fmt, const Ts &...ts)
    {
        return format(fmt.c_str(), ts...);
    }
}
