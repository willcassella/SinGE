// IFromFile.h
#pragma once

#include <Core/Util/InterfaceUtils.h>
#include <Core/Reflection/Reflection.h>
#include "../build.h"

namespace sge
{
	struct SGE_RESOURCE_API IFromFile
	{
		SGE_REFLECTED_INTERFACE
		SGE_INTERFACE_1(IFromFile, from_file)

		/////////////////////
		///   Functions   ///
	public:

		bool(*from_file)(SelfMut self, const char* path);
	};

	template <typename T>
	struct Impl< IFromFile, T >
	{
		static bool from_file(SelfMut self, const char* path)
		{
			return self.as<T>()->from_file(path);
		}
	};
}
