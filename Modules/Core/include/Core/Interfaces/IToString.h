// IToString.h
#pragma once

#include <cstring>
#include <cassert>
#include <string>
#include "../Util/InterfaceUtils.h"
#include "../Reflection/Reflection.h"

namespace sge
{
	struct SGE_CORE_API IToString
	{
		SGE_REFLECTED_INTERFACE;
		SGE_INTERFACE_1(IToString, to_string)

		/////////////////////
		///   Functions   ///
	public:

		std::string(*to_string)(Self self);
	};

	template <typename T>
	struct Impl< IToString, T >
	{
		static std::string to_string(Self self)
		{
			return self.as<T>()->to_string();
		}
	};

	template <>
	struct Impl< IToString, int8 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<int8>());
		}
	};

	template <>
	struct Impl< IToString, byte >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<byte>());
		}
	};

	template <>
	struct Impl< IToString, int16 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<int16>());
		}
	};

	template <>
	struct Impl< IToString, uint16 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<uint16>());
		}
	};

	template <>
	struct Impl< IToString, int32 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<int32>());
		}
	};

	template <>
	struct Impl< IToString, uint32 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<uint32>());
		}
	};

	template <>
	struct Impl< IToString, int64 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<int64>());
		}
	};

	template <>
	struct Impl< IToString, uint64 >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<uint64>());
		}
	};

	template <>
	struct Impl< IToString, float >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<float>());
		}
	};

	template <>
	struct Impl< IToString, double >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<double>());
		}
	};

	template <>
	struct Impl< IToString, long double >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return std::to_string(*self.as<long double>());
		}
	};

	template <>
	struct Impl< IToString, char* >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return *self.as<char*>();
		}
	};

	template <>
	struct Impl< IToString, const char* >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return *self.as<const char*>();
		}
	};

	template <std::size_t Size>
	struct Impl< IToString, char[Size] >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return *self.as<char[Size]>();
		};
	};

	template <std::size_t Size>
	struct Impl< IToString, const char[Size] >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return *self.as<const char[Size]>();
		}
	};

	template <>
	struct Impl< IToString, std::string >
	{
		static std::string to_string(Self self)
		{
			assert(!self.null());
			return *self.as<std::string>();
		}
	};
}
