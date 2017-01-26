// UpdatePipeline.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../../include/Engine/UpdatePipeline.h"
#include "../../include/Engine/SystemFrame.h"

SGE_REFLECT_TYPE(sge::UpdatePipeline);

namespace sge
{
    const UpdatePipeline::Pipeline& UpdatePipeline::get_pipeline() const
    {
        return _pipeline;
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

    void UpdatePipeline::run_tag(Any<> tag, ComponentId component, SystemFrame& callback_frame)
    {
        auto iter = _tag_callbacks.find(&tag.type());
        if (iter == _tag_callbacks.end())
        {
            return;
        }

        // Run all callbacks
        for (auto& callback : iter->second)
        {
            if (callback.component_type && callback.component_type != component.type())
            {
                continue;
            }

            if (callback.entity != NULL_ENTITY && callback.entity != component.entity())
            {
                continue;
            }

            callback.callback(callback_frame, tag, component);
        }
    }

    void UpdatePipeline::register_system_fn(std::string name, UFunction<SystemFn> system_fn)
    {
        if (name.empty())
        {
            std::cout << "Warning: Empty system names are not allowed" << std::endl;
            return;
        }

        _system_fns.insert(std::make_pair(std::move(name), std::move(system_fn)));
    }

    void UpdatePipeline::register_tag_callback(
        const TypeInfo& tag_type,
        UFunction<TagCallback::CallbackFn> callback)
    {
        TagCallback cb;
        cb.callback = std::move(callback);

        _tag_callbacks[&tag_type].push_back(std::move(cb));
    }

    void UpdatePipeline::register_tag_callback(
        const TypeInfo& tag_type,
        const TypeInfo& component_type,
        UFunction<TagCallback::CallbackFn> callback)
    {
        TagCallback cb;
        cb.component_type = &component_type;
        cb.callback = std::move(callback);

        _tag_callbacks[&tag_type].push_back(std::move(cb));
    }

    void UpdatePipeline::register_tag_callback(
        const TypeInfo& tag_type,
        ComponentId component,
        UFunction<TagCallback::CallbackFn> callback)
    {
        TagCallback cb;
        cb.component_type = component.type();
        cb.entity = component.entity();

        _tag_callbacks[&tag_type].push_back(std::move(cb));
    }
}
