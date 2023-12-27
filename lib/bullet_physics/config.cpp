#include "lib/base/interfaces/from_archive.h"
#include "lib/base/io/archive_reader.h"
#include "lib/base/reflection/reflection_builder.h"
#include "lib/bullet_physics/config.h"

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
