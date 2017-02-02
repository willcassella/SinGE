// UpdatePipeline.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../../include/Engine/UpdatePipeline.h"
#include "../../include/Engine/SystemFrame.h"
#include "../../include/Engine/Scene.h"
#include "../../include/Engine/SystemInfo.h"
#include "../../include/Engine/TagCallbackInfo.h"

SGE_REFLECT_TYPE(sge::UpdatePipeline);

namespace sge
{
    const UpdatePipeline::Pipeline& UpdatePipeline::get_pipeline() const
    {
        return _pipeline;
    }

    UpdatePipeline::AsyncToken UpdatePipeline::new_async_token()
    {
        return _next_async_token++;
    }

    UpdatePipeline::UpdatePipeline()
    {
        _next_async_token = 1;
        _next_system_token = 1;
    }

    UpdatePipeline::~UpdatePipeline()
    {
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
                auto iter = this->_systems.find(system_fn_name);
                if (iter == this->_systems.end())
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

                step.push_back(iter->second.get());
            });

            if (step.empty())
            {
                std::cout << "WARNING: Empty pipeline step." << std::endl;
                return;
            }

            this->_pipeline.push_back(std::move(step));
        });
    }

    UpdatePipeline::SystemToken UpdatePipeline::register_system_fn(
        std::string name,
        AsyncToken async_token,
        UFunction<SystemFn> system_fn)
    {
        if (name.empty())
        {
            std::cout << "Warning: Empty system names are not allowed" << std::endl;
            return NO_SYSTEM;
        }

        // Generate a system token for the system
        const auto system_token = _next_system_token++;

        // Create the system info
        auto info = std::make_unique<SystemInfo>();
        info->name = name;
        info->system_token = system_token;
        info->async_token = async_token;
        info->system_fn = std::move(system_fn);

        // Register the system
        _systems.insert(std::make_pair(std::move(name), std::move(info)));
        return system_token;
    }

    void UpdatePipeline::register_tag_callback(
        SystemToken system_token,
        AsyncToken async_token,
        TagCallbackOptions_t options,
        const TypeInfo& tag_type,
        UFunction<TagCallbackFn> callback)
    {
        // Create the tag calback info
        TagCallbackInfo info;
        info.system = system_token;
        info.options = options;
        info.callback = std::move(callback);

        // Register it
        _tag_callbacks[&tag_type][async_token].push_back(std::move(info));
    }

    void UpdatePipeline::register_tag_callback(
        SystemToken system_token,
        AsyncToken async_token,
        TagCallbackOptions_t options,
        const TypeInfo& tag_type,
        const TypeInfo& component_type,
        UFunction<TagCallbackFn> callback)
    {
        // Create the tag callback info
        TagCallbackInfo info;
        info.system = system_token,
        info.component_type = &component_type;
        info.options = options;
        info.callback = std::move(callback);

        // Register it
        _tag_callbacks[&tag_type][async_token].push_back(std::move(info));
    }
}
