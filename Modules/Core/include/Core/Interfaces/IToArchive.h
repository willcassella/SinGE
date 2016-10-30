// IToArchive.h
#pragma once

#include <cassert>
#include "../Util/InterfaceUtils.h"
#include "../Reflection/Reflection.h"
#include "../IO/ArchiveWriter.h"

namespace sge
{
	struct SGE_CORE_API IToArchive
	{
		SGE_REFLECTED_INTERFACE;
		SGE_INTERFACE_1(IToArchive, to_archive);

		/////////////////////
		///   Functions   ///
	public:

		void(*to_archive)(Self self, ArchiveWriter& writer);
	};

	template <typename T>
	struct Impl< IToArchive, T >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			self.as<T>()->to_archive(writer);
		}
	};

	template <>
	struct Impl< IToArchive, bool >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<bool>());
		}
	};

	template <>
	struct Impl< IToArchive, int8 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<int8>());
		}
	};

	template <>
	struct Impl< IToArchive, uint8 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<uint8>());
		}
	};

	template <>
	struct Impl< IToArchive, int16 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<int16>());
		}
	};

	template <>
	struct Impl< IToArchive, uint16 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<uint16>());
		}
	};

	template <>
	struct Impl< IToArchive, int32 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<int32>());
		}
	};

	template <>
	struct Impl< IToArchive, uint32 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<uint32>());
		}
	};

	template <>
	struct Impl< IToArchive, int64 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<int64>());
		}
	};

	template <>
	struct Impl< IToArchive, uint64 >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<uint64>());
		}
	};

	template <>
	struct Impl< IToArchive, float >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<float>());
		}
	};

	template <>
	struct Impl< IToArchive, double >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(*self.as<double>());
		}
	};

	template <>
	struct Impl< IToArchive, std::string >
	{
		static void to_archive(Self self, ArchiveWriter& writer)
		{
			assert(!self.null());
			writer.value(self.as<std::string>()->c_str(), self.as<std::string>()->size());
		}
	};

	template <typename T>
	void to_archive(const T& value, ArchiveWriter& writer)
	{
		Impl<IToArchive, T>::to_archive(&value, writer);
	}
}
