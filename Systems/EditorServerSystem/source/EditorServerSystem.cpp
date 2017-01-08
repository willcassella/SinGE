// EditorServerSystem.cpp

#include <chrono>
#include <Engine/Scene.h>
#include "../private/EditorServerSystemData.h"

namespace sge
{
	////////////////////////
	///   Constructors   ///

	EditorServerSystem::EditorServerSystem(uint16 port)
		: _serve_time_ms(8),
		_serve_fn_token(Scene::NULL_SYSTEM_TOKEN)
	{
		_data = std::make_unique<Data>(port);
	}

	EditorServerSystem::~EditorServerSystem()
	{
	}

	///////////////////
	///   Methods   ///

	void EditorServerSystem::register_with_scene(Scene& scene)
	{
		_serve_fn_token = scene.register_system_mut_fn(this, &EditorServerSystem::serve_fn);
	}

	void EditorServerSystem::unregister_with_scene(Scene& scene)
	{
		scene.unregister_system_fn(_serve_fn_token);
		_serve_fn_token = Scene::NULL_SYSTEM_TOKEN;
	}

	int EditorServerSystem::get_serve_time() const
	{
		return _serve_time_ms;
	}

	void EditorServerSystem::set_serve_time(int milliseconds)
	{
		_serve_time_ms = milliseconds;
	}

	void EditorServerSystem::serve_fn(SystemFrameMut& frame, float current_time, float dt)
	{
		// Create a timer so we don't run indefinetaly
		asio::steady_timer timer(_data->io);
		timer.expires_from_now(std::chrono::milliseconds{ _serve_time_ms });
		timer.async_wait([&io = _data->io](std::error_code /*error*/)
		{
			io.stop();
		});

		// Run the service
		_data->frame = &frame;
		_data->io.run();
	}
}
