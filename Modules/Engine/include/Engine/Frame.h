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

		Frame(const Scene& scene, float time)
			: _scene(nullptr), _c_scene(&scene), _time(time)
		{
		}

		Frame(Scene& scene, float time)
			: _scene(&scene), _c_scene(&scene), _time(time)
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
			return *_c_scene;
		}

		float time() const
		{
			return _time;
		}

		//////////////////
		///   Fields   ///
	private:

		Scene* _scene;
		const Scene* _c_scene;
		float _time;
	};
}
