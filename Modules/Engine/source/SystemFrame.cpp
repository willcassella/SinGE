// SystemFrame.cpp

#include <iostream>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/SystemFrame.h"
#include "../include/Engine/Scene.h"
#include "../include/Engine/UpdatePipeline.h"

SGE_REFLECT_TYPE(sge::SystemFrame);

namespace sge
{
    void SystemFrame::yield()
    {
        // Apply changes
        _scene->on_end_system_frame();

        // Execute this frame's job queue
        _scene->execute_job_queue(_job_queue.data(), _job_queue.size(), *_update_pipeline, _time_delta);
        _job_queue.clear();
    }

    uint64 SystemFrame::frame_id() const
    {
        return _scene->_frame_id;
    }

    float SystemFrame::current_time() const
    {
        return _current_time;
    }

    float SystemFrame::time_delta() const
    {
        return _time_delta;
    }

    void SystemFrame::push(const char* system_name)
    {
        auto* const system = _update_pipeline->find_system(system_name);
        if (!system)
        {
            std::cout << "Can't find system '" << system_name << "'" << std::endl;
            return;
        }

        _job_queue.push_back(system);
    }
}
