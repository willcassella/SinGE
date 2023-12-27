#pragma once

#include <memory>

#include "lib/base/io/archive.h"
#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    class SGE_RESOURCE_API JsonArchive final : public Archive
    {
        struct Data;
        SGE_REFLECTED_TYPE;

    public:
        JsonArchive();
        ~JsonArchive();

        ArchiveWriter* write_root() override;

        ArchiveReader* read_root() const override;

        bool to_file(const char* path) const override;

        bool from_file(const char* path) override;

        std::string to_string() const;

        void from_string(const char* str);

    private:
        std::unique_ptr<Data> _data;
    };
}
