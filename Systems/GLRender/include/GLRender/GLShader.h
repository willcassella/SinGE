// GLShader.h - Copyright 2013-2016 Will Cassella, All Rights Reserved
#pragma once

#include <Engine/Resources/Shader.h>
#include "config.h"

namespace sge
{
	using GLBufferID = uint32;

	struct SGE_GLRENDER_API GLShader
	{
		////////////////////////
		///   Constructors   ///
	public:

		/** Constructs a GLShader corresponding to a Shader asset. */
		GLShader(const Shader& shader);

		/** Constructs a GLShader using a file */
		GLShader(const std::string& path);

		GLShader(GLShader&& move);
		~GLShader();

		///////////////////
		///   Methods   ///
	public:

		GLBufferID get_id() const
		{
			return this->_id;
		}

		////////////////
		///   Data   ///
	private:

		GLBufferID _id;
	};
}
