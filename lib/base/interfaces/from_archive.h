#pragma once

#include <assert.h>
#include <stdint.h>

#include "lib/base/io/archive_reader.h"
#include "lib/base/reflection/reflection.h"
#include "lib/base/util/interface_utils.h"

namespace sge
{
    struct SGE_BASE_EXPORT IFromArchive
    {
        SGE_REFLECTED_INTERFACE;
        SGE_INTERFACE_1(IFromArchive, from_archive);

        void (*from_archive)(SelfMut self, ArchiveReader &reader);
    };

    template <typename T>
    struct Impl<IFromArchive, T>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            self.as<T>()->from_archive(reader);
        }
    };

    template <>
    struct Impl<IFromArchive, bool>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.boolean(*self.as<bool>());
        }
    };

    template <>
    struct Impl<IFromArchive, int8_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<int8_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, uint8_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint8_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, int16_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<int16_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, uint16_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint16_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, int32_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<int32_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, uint32_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint32_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, int64_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<int64_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, uint64_t>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint64_t>());
        }
    };

    template <>
    struct Impl<IFromArchive, float>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<float>());
        }
    };

    template <>
    struct Impl<IFromArchive, double>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.number(*self.as<double>());
        }
    };

    template <>
    struct Impl<IFromArchive, std::string>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());

            size_t len = 0;
            if (reader.string_size(len))
            {
                self.as<std::string>()->assign(len, 0);
                reader.string(&(*self.as<std::string>())[0], len);
            }
        }
    };

    template <size_t Size>
    struct Impl<IFromArchive, char[Size]>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.string(*self.as<char[Size]>(), Size);
        }
    };

    template <typename T, size_t Size>
    struct Impl<IFromArchive, T[Size]>
    {
        static void from_archive(SelfMut self, ArchiveReader &reader)
        {
            assert(!self.null());
            reader.typed_array(*self.as<T[Size]>(), Size);
        }
    };

    template <typename T>
    void from_archive(T &value, ArchiveReader &reader)
    {
        Impl<IFromArchive, T>::from_archive(&value, reader);
    }

    template <typename T>
    void fields_from_archive(T &value, ArchiveReader &reader)
    {
        reader.enumerate_object_members([&](const char *name)
                                        {
            // Search for the field being assigned
            const auto* field = get_type<T>().find_field(name);

            // If the field doesn't exist, or it's not meant to be written to
            if (!field || (field->flags() & FF_READONLY))
            {
                return;
            }

            // Get the FromArchive vtable for this field
            const auto* fieldImpl = get_vtable<IFromArchive>(field->type());

            // If this field type doesn't support deserialization, continue
            if (!fieldImpl)
            {
                return;
            }

            // Deserialize the field
            auto* fieldValue = field->get(&value).object();
            fieldImpl->from_archive(fieldValue, reader); });
    }
}
