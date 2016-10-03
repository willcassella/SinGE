// IToString.h
#pragma once

#include <cstring>
#include <string>
#include "../Reflection/Reflection.h"

namespace sge
{
	struct SGE_CORE_API IToString
	{
		SGE_REFLECTED_INTERFACE;
		SGE_VTABLE_1(IToString, to_string);

		/////////////////////
		///   Functions   ///
	public:

		std::string(*to_string)(const void* self);
	};

	template <typename T>
	struct Impl< IToString, T >
	{
		static std::string to_string(const void* self)
		{
			return static_cast<const T*>(self)->to_string();
		}
	};

	template <>
	struct Impl< IToString, byte >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const byte*>(self));
		}
	};

	template <>
	struct Impl< IToString, int16 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const int16*>(self));
		}
	};

	template <>
	struct Impl< IToString, uint16 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const uint16*>(self));
		}
	};

	template <>
	struct Impl< IToString, int32 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const int32*>(self));
		}
	};

	template <>
	struct Impl< IToString, uint32 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const uint32*>(self));
		}
	};

	template <>
	struct Impl< IToString, int64 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const int64*>(self));
		}
	};

	template <>
	struct Impl< IToString, uint64 >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const uint64*>(self));
		}
	};

	template <>
	struct Impl< IToString, float >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const float*>(self));
		}
	};

	template <>
	struct Impl< IToString, double >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const double*>(self));
		}
	};

	template <>
	struct Impl< IToString, long double >
	{
		static std::string to_string(const void* self)
		{
			return std::to_string(*static_cast<const long double*>(self));
		}
	};

	template <typename T>
	std::string to_string(const T& value)
	{
		return Impl<IToString, T>::to_string(&value);
	}

	inline void format_impl(std::string& out, const char* fmt, std::size_t len)
	{
		out.append(fmt, len);
	}

	template <typename T, typename ... Ts>
	void format_impl(std::string& out, const char* fmt, std::size_t len, const T& t, const Ts& ... ts)
	{
		for (; len > 0; ++fmt, --len)
		{
			if (*fmt == '@')
			{
				out += to_string(t);
				return format_impl(out, fmt + 1, len - 1, ts...);
			}
			else
			{
				out += *fmt;
			}
		}
	}

	template <typename ... Ts>
	std::string format(const char* fmt, const Ts& ... ts)
	{
		std::string result;
		format_impl(result, fmt, std::strlen(fmt), ts...);
		return result;
	}

	template <typename ... Ts>
	std::string format(const std::string& fmt, const Ts& ... ts)
	{
		std::string result;
		format_impl(result, fmt.c_str(), fmt.length(), ts...);
		return result;
	}
}
