// Color.cpp

#include <sstream>
#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Interfaces/IToString.h>
#include <Core/Interfaces/IFromString.h>
#include <Core/Interfaces/IToArchive.h>
#include <Core/Interfaces/IFromArchive.h>
#include "../../include/Resource/Misc/Color.h"

SGE_REFLECT_TYPE(sge::color::RGBA8)
.flags(TF_RECURSE_TERMINAL)
.implements<IToString>()
.implements<IFromString>()
.implements<IToArchive>()
.implements<IFromArchive>()
.property("red", &RGBA8::red, &RGBA8::red)
.property("green", &RGBA8::green, &RGBA8::green)
.property("blue", &RGBA8::blue, &RGBA8::blue)
.property("alpha", &RGBA8::alpha, &RGBA8::alpha);

namespace sge
{
	namespace color
	{
		std::string RGBA8::to_string() const
		{
			std::stringstream result;
			result << std::hex << color;
			return result.str();
		}

		std::size_t RGBA8::from_string(const char* str, std::size_t /*len*/)
		{
			char* end = nullptr;
			color = std::strtoul(str, &end, 16);
			return end - str;
		}

		void RGBA8::to_archive(ArchiveWriter& writer) const
		{
			sge::to_archive(to_string(), writer);
		}

		void RGBA8::from_archive(ArchiveReader& reader)
		{
			// Get the length of the string
			std::size_t len = 0;
			if (!reader.string_size(len))
			{
				return;
			}

			// Create a buffer to hold the string
			char* buff = SGE_STACK_ALLOC(char, len + 1);
			reader.string(buff, len);
			buff[len] = 0;

			// Set the value from the string
			color = std::strtoul(buff, nullptr, 16);
		}
	}
}
