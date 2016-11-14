// EditorServerSystem.cpp

#include <chrono>
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

	void EditorServerSystem::serve(int milliseconds)
	{
		// Create a timer so we don't run indefinetaly
		asio::steady_timer timer(_data->io);
		timer.expires_from_now(std::chrono::milliseconds{ milliseconds });
		timer.async_wait([&io = _data->io](const std::error_code&)
		{
			io.stop();
		});

		// Run the service
		_data->io.run();
	}
}
