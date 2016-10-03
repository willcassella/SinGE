// FieldInfo.h
#pragma once

#include <string>
#include "Any.h"

namespace sge
{
	struct TypeInfo;

	enum FieldFlags
	{
		/**
		 * \brief This field has no special flags.
		 */
		FF_NONE = 0,

		/**
		 * \brief This field is part of the public interface of the type, and may be accessed by scripts and the editor.
		 */
		FF_PUBLIC = (1 << 0),

		/**
		 * \breif This field is not serialized by the 'to_archive' and 'from_archive' functions. (By default, all fields are).
		 */
		FF_TRANSIENT = (1 << 1),

		/**
		 * \breif This field should not be written to.
		 */
		FF_READONLY = (1 << 2) | FF_TRANSIENT
	};

	struct FieldInfo
	{
		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data()
				: flags(FF_NONE), type(nullptr), offset(0)
			{
			}

			//////////////////
			///   Fields   ///
		public:

			FieldFlags flags;
			const TypeInfo* type;
			std::size_t offset;
		};

		////////////////////////
		///   Constructors   ///
	public:

		FieldInfo(Data data)
			: _data(std::move(data))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		/**
		 * \breif Returns the flags assigned to this field.
		 */
		FieldFlags flags() const
		{
			return _data.flags;
		}

		/**
		 * \breif Returns the type of this field.
		 */
		const TypeInfo& type() const
		{
			return *_data.type;
		}

		/**
		 * \breif Returns the offset of this field from the base object pointer.
		 */
		std::size_t offset() const
		{
			return _data.offset;
		}

		/**
		 * \brief Accesses this field.
		 * \param self The instance to access the field on.
		 * \return A pointer the field.
		 */
		Any get(const void* self) const
		{
			return{ static_cast<const char*>(self) + _data.offset, *_data.type };
		}

		/**
		 * \brief Accesses this field.
		 * \param self The instance to access the field on.
		 * \return A pointer to the field.
		 */
		AnyMut get(void* self) const
		{
			return{ static_cast<char*>(self) + _data.offset, *_data.type };
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}
