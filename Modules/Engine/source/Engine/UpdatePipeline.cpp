// UpdatePipeline.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../../include/Engine/UpdatePipeline.h"
#include "../../include/Engine/SystemFrame.h"
#include "../../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::UpdatePipeline);

namespace sge
{
    const UpdatePipeline::Pipeline& UpdatePipeline::get_pipeline() const
    {
        return _pipeline;
    }

    UpdatePipeline::AsyncToken UpdatePipeline::get_async_token()
    {
        return _next_async_token++;
    }

    UpdatePipeline::UpdatePipeline()
    {
        _next_async_token = 1;
        _next_system_token = 1;
    }

    void UpdatePipeline::configure_pipeline(ArchiveReader& reader)
    {
        _pipeline.clear();

        reader.enumerate_array_elements([this, &reader](std::size_t /*i*/)
        {
            PipelineStep step;

            reader.enumerate_array_elements([this, &reader, &step](std::size_t /*i*/)
            {
                // Get the name of the system
                std::string system_fn_name;
                sge::from_archive(system_fn_name, reader);

                // Search for the system
                auto iter = this->_system_fns.find(system_fn_name);
                if (iter == this->_system_fns.end())
                {
                    if (system_fn_name.empty())
                    {
                        std::cout << "WARNING: Empty pipeline system function name." << std::endl;
                    }
                    else
                    {
                        std::cout << "WARNING: Invalid pipeline system function name: '" << system_fn_name << "'" << std::endl;
                    }

                    return;
                }

                // TODO: Develope a copyable replacement for std::function
                step.push_back(std::move(iter->second));
            });

            if (step.empty())
            {
                std::cout << "WARNING: Empty pipeline step." << std::endl;
                return;
            }

            this->_pipeline.push_back(std::move(step));
        });
    }

    UpdatePipeline::SystemToken UpdatePipeline::register_system_fn(std::string name, UFunction<SystemFn> system_fn)
    {
        if (name.empty())
        {
            std::cout << "Warning: Empty system names are not allowed" << std::endl;
            return NO_SYSTEM;
        }

        _system_fns.insert(std::make_pair(std::move(name), std::move(system_fn)));
        return _next_system_token++;
    }

    void UpdatePipeline::register_tag_callback(
        SystemToken system_token,
        AsyncToken async_token,
        TagCallbackOptions_t options,
        const TypeInfo& tag_type,
        UFunction<TagCallbackFn> callback)
    {
        TagCallback cb;
        cb.system = system_token;
        cb.options = options;
        cb.callback = std::move(callback);

        _tag_callbacks[&tag_type][async_token].push_back(std::move(cb));
    }

    void UpdatePipeline::register_tag_callback(
        SystemToken system_token,
        AsyncToken async_token,
        TagCallbackOptions_t options,
        const TypeInfo& tag_type,
        const TypeInfo& component_type,
        UFunction<TagCallbackFn> callback)
    {
        TagCallback cb;
        cb.system = system_token,
        cb.component_type = &component_type;
        cb.options = options;
        cb.callback = std::move(callback);

        _tag_callbacks[&tag_type][async_token].push_back(std::move(cb));
    }
}
