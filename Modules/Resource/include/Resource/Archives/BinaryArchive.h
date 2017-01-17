// BinaryArchive.h
#pragma once

#include <vector>
#include <Core/Reflection/Reflection.h>
#include <Core/IO/Archive.h>
#include "../build.h"

namespace sge
{
    class SGE_RESOURCE_API BinaryArchive final : public Archive
    {
        SGE_REFLECTED_TYPE;

        ///////////////////
        ///   Methods   ///
    public:

        ArchiveReader* read_root() const override;

        ArchiveWriter* write_root() override;

        bool to_file(const char* path) const override;

        bool from_file(const char* path) override;

        const std::vector<byte>& buffer() const
        {
            return _buffer;
        }

        //////////////////
        ///   Fields   ///
    private:

        std::vector<byte> _buffer;
    };
}
