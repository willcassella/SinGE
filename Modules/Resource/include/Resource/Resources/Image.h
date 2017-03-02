// Image.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include "../Misc/Color.h"

struct FIBITMAP;

namespace sge
{
	class ArchiveWriter;

	struct SGE_RESOURCE_API Image
	{
		SGE_REFLECTED_TYPE;


		////////////////////////
		///   Constructors   ///
	public:

		Image();
		Image(const std::string& path);
		Image(const Image& copy);
		Image(Image&& move);
		~Image();

		///////////////////
		///   Methods   ///
	public:

        static void save_rgbf(const float* image, int32 width, int32 height, byte num_channels, const char* path);

        static void dilate_rgbf(const float* image, int32 width, int32 height, byte num_channels, float* out);

        static void smooth_rgbf(const float* image, int32 width, int32 height, byte num_channels, float* out);

		bool from_file(const char* path);

		void to_archive(ArchiveWriter& archive) const;

		uint32 get_width() const;

		uint32 get_height() const;

		byte* get_bitmap();

		const byte* get_bitmap() const;

		//////////////////
		///   Fields   ///
	private:

		FIBITMAP* _bitmap;
	};
}
