// ISystem.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../config.h"

namespace sge
{
	struct Scene;

	struct SGE_ENGINE_API ISystem final
	{
		SGE_REFLECTED_INTERFACE;
		SGE_VTABLE_1(ISystem, init_selectors);

		/////////////////////
		///   Functions   ///
	public:

		void(*init_selectors)(const void*, const Scene&);
	};

	template <typename T>
	struct Impl < ISystem, T >
	{
		static void init_selectors(const void* self, const Scene& scene)
		{
			static_cast<const T*>(self)->init_selectors(scene);
		}
	};
}
