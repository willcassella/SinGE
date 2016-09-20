// FieldInfo.h
#pragma once

#include <string>
#include "../config.h"

namespace sge
{
	struct TypeInfo;

	enum FieldFlags
	{
		FF_NONE = 0,

		/* This field is part of the public interface of the type, and may be accesed by scripts and the editor. */
		FF_PUBLIC = (1 << 0),

		/* This field is not serialized by the 'to_archive' and 'from_archive' functions. (By default, all fields are). */
		FF_TRANSIENT = (1 << 1),

		/* This field should not be written to. */
		FF_READONLY = (1 << 2) | FF_TRANSIENT
	};

	struct FieldInfo
	{
		////////////////////////
		///   Constructors   ///
	public:

		FieldInfo(const TypeInfo* type, FieldFlags flags, std::size_t offset)
			: _type{ type }, _flags{ flags }, _offset{ offset }
		{
		}

		//////////////////
		///   Fields   ///
	private:

		const TypeInfo* _type;
		FieldFlags _flags;
		std::size_t _offset;
	};
}
