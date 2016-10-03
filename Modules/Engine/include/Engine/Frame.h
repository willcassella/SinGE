// Frame.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "config.h"

namespace sge
{
	struct Scene;

	struct SGE_ENGINE_API Frame
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		Frame(Scene& scene, float time)
			: _scene(&scene), _time(time)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		Scene& scene()
		{
			return *_scene;
		}

		const Scene& scene() const
		{
			return *_scene;
		}

		float time() const
		{
			return _time;
		}

		//////////////////
		///   Fields   ///
	private:

		Scene* _scene;
		float _time;
	};
}
