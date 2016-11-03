// EditorServerSystem.h
#pragma once

#include <memory>
#include <Engine/Scene.h>
#include "config.h"

namespace sge
{
	struct SGE_EDITORSERVERSYSTEM_API EditorServerSystem
	{
		struct Data;

		SGE_REFLECTED_TYPE;

		EditorServerSystem(Scene& scene, uint16 port);
		~EditorServerSystem();

		///////////////////
		///   Methods   ///
	public:

		void serve();

		//////////////////
		///   Fields   ///
	private:

		std::unique_ptr<Data> _data;
	};
}
