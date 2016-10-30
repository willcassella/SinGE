// IFromString.h
#pragma once

#include <cassert>
#include <string>
#include "../Util/InterfaceUtils.h"
#include "../Reflection/Reflection.h"

namespace sge
{
	struct SGE_CORE_API IFromString
	{
		SGE_REFLECTED_INTERFACE;
		SGE_INTERFACE_1(IFromString, from_string)

		/////////////////////
		///   Functions   ///
	public:

		std::size_t(*from_string)(SelfMut self, const std::string& str);
	};

	template <typename T>
	struct Impl< IFromString, T >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			return self.as<T>()->from_string(str);
		}
	};

	template <>
	struct Impl< IFromString, int32 >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			assert(!self.null());
			std::size_t index;
			*self.as<int32>() = std::stoi(str, &index);
			return index;
		}
	};

	template <>
	struct Impl< IFromString, int64 >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			assert(!self.null());
			std::size_t index;
			*self.as<int64>() = std::stoll(str, &index);
			return index;
		}
	};

	template <>
	struct Impl< IFromString, uint64 >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			assert(!self.null());
			std::size_t index;
			*self.as<uint64>() = std::stoull(str, &index);
			return index;
		}
	};

	template <>
	struct Impl< IFromString, float >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			assert(!self.null());
			std::size_t index;
			*self.as<float>() = std::stof(str, &index);
			return index;
		}
	};

	template <>
	struct Impl< IFromString, double >
	{
		static std::size_t from_string(SelfMut self, const std::string& str)
		{
			assert(!self.null());
			std::size_t index;
			*self.as<double>() = std::stod(str, &index);
			return index;
		}
	};

	template <typename T>
	std::size_t from_string(T& self, const std::string& str)
	{
		return Impl<IFromString, T>::from_string(&self, str);
	}
}
