// Config.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/IO/ArchiveReader.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../include/BulletPhysics/Config.h"

SGE_REFLECT_TYPE(sge::bullet_physics::Config)
.implements<IFromArchive>();

namespace sge
{
    namespace bullet_physics
    {
        Config::Config()
            : global_gravity{ 0, -10, 0 }
        {
        }

        bool Config::validate() const
        {
            return true;
        }

        void Config::from_archive(ArchiveReader& reader)
        {
            reader.object_member("gravity", global_gravity);
        }
    }
}
