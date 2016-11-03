// EditorServerSystem.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Resource/Archives/JsonArchive.h>
#include "../private/EditorServerSystemData.h"

SGE_REFLECT_TYPE(sge::EditorServerSystem);

namespace sge
{
	EditorServerSystem::EditorServerSystem(Scene& scene, uint16 port)
	{
		_data = std::make_unique<Data>(scene, port);
	}

	EditorServerSystem::~EditorServerSystem()
	{
	}

	void EditorServerSystem::serve()
	{
		_data->io.run();
	}
}
