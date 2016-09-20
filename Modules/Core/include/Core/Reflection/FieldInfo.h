// FieldInfo.h
#pragma once

#include <string>
#include "../config.h"

namespace sge
{
	struct TypeInfo;

	template <typename T>
	const TypeInfo& get_type();

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

	struct SGE_CORE_API FieldInfo
	{
		////////////////////////
		///   Constructors   ///
	public:

		template <typename T>
		static FieldInfo create(std::string name, FieldFlags flags, std::size_t offset)
		{
			FieldInfo result;
			result.name = std::move(name);
			result.type = &get_type<T>();
			result.flags = flags;
			result.offset = offset;

			return result;
		}

		//////////////////
		///   Fields   ///
	public:

		/* The name of this field. */
		std::string name;

		/* The type this field is. */
		const TypeInfo* type;

		/* Flags assigned to this field. */
		FieldFlags flags;

		/* The offset of this field within the owning object. */
		std::size_t offset;
	};
}
