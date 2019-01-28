// JsonArchive.h
#pragma once

#include <memory>
#include <Core/IO/Archive.h>
#include <Core/Reflection/Reflection.h>
#include "../build.h"

namespace sge
{
    class SGE_RESOURCE_API JsonArchive final : public Archive
    {
        struct Data;
        SGE_REFLECTED_TYPE;

        ////////////////////////
        ///   Constructors   ///
    public:

        JsonArchive();
        ~JsonArchive();

        ///////////////////
        ///   Methods   ///
    public:

        ArchiveWriter* write_root() override;

        ArchiveReader* read_root() const override;

        bool to_file(const char* path) const override;

        bool from_file(const char* path) override;

        std::string to_string() const;

        void from_string(const char* str);

        //////////////////
        ///   Fields   ///
    private:

        std::unique_ptr<Data> _data;
    };
}
