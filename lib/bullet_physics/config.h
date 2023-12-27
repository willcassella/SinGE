#pragma once

#include "lib/base/math/vec3.h"
#include "lib/base/reflection/reflection.h"
#include "lib/bullet_physics/build.h"

namespace sge
{
    class ArchiveReader;

    namespace bullet_physics
    {
        struct SGE_BULLET_PHYSICS_API Config
        {
            SGE_REFLECTED_TYPE;

            Config();

            bool validate() const;

            void from_archive(ArchiveReader& reader);

            Vec3 global_gravity;
        };
    }
}
