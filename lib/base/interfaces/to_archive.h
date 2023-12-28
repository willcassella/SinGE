#pragma once

#include <assert.h>
#include <map>
#include <stdint.h>
#include <unordered_map>

#include "lib/base/io/archive_writer.h"
#include "lib/base/reflection/reflection.h"
#include "lib/base/util/interface_utils.h"

namespace sge
{
    struct SGE_BASE_EXPORT IToArchive
    {
        SGE_REFLECTED_INTERFACE;
        SGE_INTERFACE_1(IToArchive, to_archive)

        void (*to_archive)(Self self, ArchiveWriter &writer);
    };

    template <typename T>
    struct Impl<IToArchive, T>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            self.as<T>()->to_archive(writer);
        }
    };

    template <>
    struct Impl<IToArchive, bool>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.boolean(*self.as<bool>());
        }
    };

    template <>
    struct Impl<IToArchive, int8_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<int8_t>());
        }
    };

    template <>
    struct Impl<IToArchive, uint8_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<uint8_t>());
        }
    };

    template <>
    struct Impl<IToArchive, int16_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<int16_t>());
        }
    };

    template <>
    struct Impl<IToArchive, uint16_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<uint16_t>());
        }
    };

    template <>
    struct Impl<IToArchive, int32_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<int32_t>());
        }
    };

    template <>
    struct Impl<IToArchive, uint32_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<uint32_t>());
        }
    };

    template <>
    struct Impl<IToArchive, int64_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<int64_t>());
        }
    };

    template <>
    struct Impl<IToArchive, uint64_t>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<uint64_t>());
        }
    };

    template <>
    struct Impl<IToArchive, float>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<float>());
        }
    };

    template <>
    struct Impl<IToArchive, double>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.number(*self.as<double>());
        }
    };

    template <>
    struct Impl<IToArchive, std::string>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.string(self.as<std::string>()->c_str(), self.as<std::string>()->size());
        }
    };

    template <size_t Size>
    struct Impl<IToArchive, char[Size]>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.string(*self.as<char[Size]>(), Size);
        }
    };

    template <size_t Size>
    struct Impl<IToArchive, const char[Size]>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.string(*self.as<const char[Size]>(), Size);
        }
    };

    template <typename T, size_t Size>
    struct Impl<IToArchive, T[Size]>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());
            writer.typed_array(*self.as<T[Size]>(), Size);
        }
    };

    template <typename T, typename HasherT, typename KeyEqT, typename AllocT>
    struct Impl<IToArchive, std::unordered_map<std::string, T, HasherT, KeyEqT, AllocT>>
    {
        static void to_archive(Self self, ArchiveWriter &writer)
        {
            assert(!self.null());

            for (const auto &entry : *self.as<std::unordered_map<std::string, HasherT, KeyEqT, AllocT>>())
            {
                writer.push_object_member(entry.first.c_str(), entry.second);
            }
        }
    };

    template <typename T>
    void to_archive(const T &value, ArchiveWriter &writer)
    {
        Impl<IToArchive, T>::to_archive(&value, writer);
    }

    template <typename T>
    void fields_to_archive(const T &value, ArchiveWriter &writer)
    {
        get_type<T>().enumerate_fields([&](const char *name, const FieldInfo &field)
                                       {
            // If this field isn't meant to be serialized, continue
            if (field.flags() & FF_TRANSIENT)
            {
                return;
            }

            // Get the ToArchive implementation for this field
            const auto* fieldImpl = get_vtable<IToArchive>(field.type());

            // If this field type doesn't support serialization, continue
            if (!fieldImpl)
            {
                return;
            }

            // Get the value of the field
            auto* fieldValue = field.get(&value).object();

            // Serialize the field
            writer.push_object_member(name);
            fieldImpl->to_archive(fieldValue, writer);
            writer.pop(); });
    }
}
