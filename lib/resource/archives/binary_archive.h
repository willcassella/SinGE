#pragma once

#include <stdint.h>
#include <vector>

#include "lib/base/io/archive.h"
#include "lib/base/reflection/reflection.h"
#include "lib/resource/build.h"

namespace sge
{
    class SGE_RESOURCE_API BinaryArchive final : public Archive
    {
        SGE_REFLECTED_TYPE;

    public:
        ArchiveReader* read_root() const override;

        ArchiveWriter* write_root() override;

        bool to_file(const char* path) const override;

        bool from_file(const char* path) override;

        const std::vector<uint8_t>& buffer() const
        {
            return _buffer;
        }

    private:
        std::vector<uint8_t> _buffer;
    };
}
