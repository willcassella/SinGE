// EditorServerSystem.cpp

#include <chrono>
#include <Engine/UpdatePipeline.h>
#include "../private/EditorServerSystemData.h"

namespace sge
{
	////////////////////////
	///   Constructors   ///

	EditorServerSystem::EditorServerSystem(uint16 port)
		: _serve_time_ms(8)
	{
		_data = std::make_unique<Data>(port);
	}

	EditorServerSystem::~EditorServerSystem()
	{
	}

	///////////////////
	///   Methods   ///

	void EditorServerSystem::register_pipeline(UpdatePipeline& pipeline)
	{
        auto async_token = pipeline.new_async_token();
        pipeline.register_system_fn(
            "editor_server_serve",
            async_token,
            this,
            &EditorServerSystem::serve_fn);
	}

	int EditorServerSystem::get_serve_time() const
	{
		return _serve_time_ms;
	}

	void EditorServerSystem::set_serve_time(int milliseconds)
	{
		_serve_time_ms = milliseconds;
	}

	void EditorServerSystem::serve_fn(SystemFrame& frame, float current_time, float dt)
	{
		// Run the service
		_data->frame = &frame;
        _data->io.poll();
	}
}
