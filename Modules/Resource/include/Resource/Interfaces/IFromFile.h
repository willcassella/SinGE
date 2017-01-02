// IFromFile.h
#pragma once

#include <Core/Util/InterfaceUtils.h>
#include <Core/Reflection/Reflection.h>
#include "../config.h"

namespace sge
{
	struct SGE_RESOURCE_API IFromFile
	{
		SGE_REFLECTED_INTERFACE
		SGE_INTERFACE_1(IFromFile, from_file)

		/////////////////////
		///   Functions   ///
	public:

		void(*from_file)(SelfMut self, const char* path);
	};

	template <typename T>
	struct Impl< IFromFile, T >
	{
		static void from_file(SelfMut self, const char* path)
		{
			self.as<T>()->from_file(path);
		}
	};
}
