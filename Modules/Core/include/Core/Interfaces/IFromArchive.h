// IFromArchive.h
#pragma once

#include <cassert>
#include "../Util/InterfaceUtils.h"
#include "../Reflection/Reflection.h"
#include "../IO/ArchiveReader.h"

namespace sge
{
    struct SGE_CORE_API IFromArchive
    {
        SGE_REFLECTED_INTERFACE;
        SGE_INTERFACE_1(IFromArchive, from_archive);

        /////////////////////
        ///   Functions   ///
    public:

        void(*from_archive)(SelfMut self, ArchiveReader& reader);
    };

    template <typename T>
    struct Impl< IFromArchive, T >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            self.as<T>()->from_archive(reader);
        }
    };

    template <>
    struct Impl< IFromArchive, bool >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.boolean(*self.as<bool>());
        }
    };

    template <>
    struct Impl< IFromArchive, int8 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<int8>());
        }
    };

    template <>
    struct Impl< IFromArchive, uint8 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint8>());
        }
    };

    template <>
    struct Impl< IFromArchive, int16 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<int16>());
        }
    };

    template <>
    struct Impl< IFromArchive, uint16 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint16>());
        }
    };

    template <>
    struct Impl< IFromArchive, int32 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<int32>());
        }
    };

    template <>
    struct Impl< IFromArchive, uint32 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint32>());
        }
    };

    template <>
    struct Impl< IFromArchive, int64 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<int64>());
        }
    };

    template <>
    struct Impl< IFromArchive, uint64 >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<uint64>());
        }
    };

    template <>
    struct Impl< IFromArchive, float >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<float>());
        }
    };

    template <>
    struct Impl< IFromArchive, double >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.number(*self.as<double>());
        }
    };

    template <>
    struct Impl< IFromArchive, std::string >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());

            std::size_t len = 0;
            if (reader.string_size(len))
            {
                self.as<std::string>()->assign(len, 0);
                reader.string(&(*self.as<std::string>())[0], len);
            }
        }
    };

    template <std::size_t Size>
    struct Impl< IFromArchive, char[Size] >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.string(*self.as<char[Size]>(), Size);
        }
    };

    template <typename T, std::size_t Size>
    struct Impl< IFromArchive, T[Size] >
    {
        static void from_archive(SelfMut self, ArchiveReader& reader)
        {
            assert(!self.null());
            reader.typed_array(*self.as<T[Size]>(), Size);
        }
    };

    template <typename T>
    void from_archive(T& value, ArchiveReader& reader)
    {
        Impl<IFromArchive, T>::from_archive(&value, reader);
    }

    template <typename T>
    void fields_from_archive(T& value, ArchiveReader& reader)
    {
        reader.enumerate_object_members([&](const char* name)
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
            fieldImpl->from_archive(fieldValue, reader);
        });
    }
}
