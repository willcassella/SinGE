// GLRenderSystemConfig.h
#pragma once

#include <string>
#include <Core/Reflection/Reflection.h>
#include "build.h"

namespace sge
{
	class ArchiveReader;

	namespace gl_render
	{
		struct SGE_GLRENDER_API Config
		{
			SGE_REFLECTED_TYPE;

			////////////////////////
			///   Constructors   ///
		public:

			Config();

			///////////////////
			///   Methods   ///
		public:

			void from_archive(ArchiveReader& reader);

			bool validate() const;

			//////////////////
			///   Fields   ///
		public:

			int viewport_width;

			int viewport_height;

			std::string viewport_vert_shader;

			std::string viewport_frag_shader;

            std::string debug_line_vert_shader;

            std::string debug_line_frag_shader;

			std::string missing_material;

			std::string missing_mesh;
		};
	}
}
