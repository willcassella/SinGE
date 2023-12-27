#pragma once

#include <memory>

#include "lib/base/reflection/reflection.h"
#include "lib/editor_server/build.h"
#include "lib/engine/scene.h"

namespace sge
{
    struct SGE_EDITORSERVERSYSTEM_API EditorServerSystem
    {
        struct Data;
        SGE_REFLECTED_TYPE;

        EditorServerSystem(uint16 port);
        ~EditorServerSystem();

        void register_pipeline(UpdatePipeline& pipeline);

        int get_serve_time() const;

        void set_serve_time(int milliseconds);

    private:
        void serve_fn(Scene& scene, SystemFrame& frame);

        int _serve_time_ms;
        std::unique_ptr<Data> _data;
    };
}
