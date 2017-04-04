// UpdatePipeline.h
#pragma once

#include <stack>
#include <memory>
#include <Core/Reflection/Reflection.h>
#include <Core/Functional/UFunction.h>
#include "config.h"

namespace sge
{
	class ArchiveReader;
    struct Scene;
    struct SceneData;
    struct SystemFrame;
    struct SystemInfo;

    struct SGE_ENGINE_API UpdatePipeline
    {
        SGE_REFLECTED_TYPE;
        friend Scene;
        using SystemFn = void(Scene& scene, SystemFrame& frame);
        using Pipeline = std::vector<SystemInfo*>;

        /////////////////////////
        ///   Constructors    ///
    public:

        UpdatePipeline();
        ~UpdatePipeline();
        UpdatePipeline(const UpdatePipeline& copy) = delete;
        UpdatePipeline& operator=(const UpdatePipeline& copy) = delete;
        UpdatePipeline(UpdatePipeline&& move) = delete;
        UpdatePipeline& operator=(UpdatePipeline&& move) = delete;

        ///////////////////
        ///   Methods   ///
    public:

        void configure_pipeline(ArchiveReader& reader);

        const Pipeline& get_pipeline() const;

		void register_system_fn(
			std::string name,
			UFunction<SystemFn> system_fn);

		template <class ObjT, typename SystemFnT>
		void register_system_fn(
			std::string name,
			ObjT* obj,
			SystemFnT system_fn)
		{
			auto wrapper = [obj, fn = std::move(system_fn)](Scene& scene, SystemFrame& frame)
			{
				(obj->*fn)(scene, frame);
			};

			register_system_fn(std::move(name), std::move(wrapper));
		}

		SystemInfo* find_system(const char* name);

        //////////////////
        ///   Fields   ///
    private:

		/* Frame update pipeline. */
        Pipeline _pipeline;

        /* System functions */
        std::unordered_map<std::string, std::unique_ptr<SystemInfo>> _systems;
    };
}
