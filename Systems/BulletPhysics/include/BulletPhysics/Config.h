// Config.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include <Core/Math/Vec3.h>
#include "build.h"

namespace sge
{
    class ArchiveReader;

    namespace bullet_physics
    {
        struct SGE_BULLET_PHYSICS_API Config
        {
            SGE_REFLECTED_TYPE;

            ////////////////////////
            ///   Constructors   ///
        public:

            Config();

            ///////////////////
            ///   Methods   ///
        public:

            bool validate() const;

            void from_archive(ArchiveReader& reader);

            //////////////////
            ///   Fields   ///
        public:

            Vec3 global_gravity;
        };
    }
}
