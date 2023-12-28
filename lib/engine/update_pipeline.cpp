#include <iostream>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/system_info.h"
#include "lib/engine/update_pipeline.h"

SGE_REFLECT_TYPE(sge::UpdatePipeline);

namespace sge
{
    UpdatePipeline::UpdatePipeline() = default;

    UpdatePipeline::~UpdatePipeline() = default;

    const UpdatePipeline::Pipeline& UpdatePipeline::get_pipeline() const
    {
        return _pipeline;
    }

    void UpdatePipeline::configure_pipeline(ArchiveReader& reader)
    {
        _pipeline.clear();

        reader.enumerate_array_elements([this, &reader](size_t /*i*/)
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

            this->_pipeline.push_back(iter->second.get());
        });
    }

    void UpdatePipeline::register_system_fn(
        std::string name,
        UFunction<SystemFn> system_fn)
    {
        if (name.empty())
        {
            std::cout << "Warning: Empty system names are not allowed" << std::endl;
        }

        // Create the system info
        auto info = std::make_unique<SystemInfo>();
        info->name = name;
        info->system_fn = std::move(system_fn);

        // Register the system
        _systems.insert(std::make_pair(std::move(name), std::move(info)));
    }

    SystemInfo* UpdatePipeline::find_system(const char* name)
    {
        const auto iter = _systems.find(name);
        return iter != _systems.end() ? iter->second.get() : nullptr;
    }
}
