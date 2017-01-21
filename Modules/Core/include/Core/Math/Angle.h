// Angle.h
#pragma once

#include "../Reflection/Reflection.h"
#include "../Reflection/ReflectionBuilder.h"
#include "../Interfaces/IToArchive.h"
#include "../Interfaces/IFromArchive.h"

namespace sge
{
	/** An arithmetic value representing an angle (internally stored as radians). */
	template <typename T>
	struct TAngle final
	{
		/////////////////////
		///   Constants   ///
	public:

		/** Multiplication factor for converting degrees to radians. */
		static constexpr T Degrees_To_Radians = T{ 0.0174533f };

		/** Multiplication factor for converting radians to degrees. */
		static constexpr T Radians_To_Degrees = T{ 57.2958f };

		////////////////////////
		///   Constructors   ///
	public:

		constexpr TAngle()
			: _radians{ 0 }
		{
		}
		constexpr TAngle(T radians)
			: _radians{ radians }
		{
		}

		///////////////////
		///   Methods   ///
	public:

        void to_archive(ArchiveWriter& writer) const
        {
            writer.number(_radians);
        }

        void from_archive(ArchiveReader& reader)
        {
            reader.number(_radians);
        }

		/** Returns the current value in radians. */
		constexpr T radians() const
		{
			return _radians;
		}

		/** Sets the current value in radians. */
		constexpr void radians(T value)
		{
			_radians = value;
		}

		/** Returns the current value in degrees. */
		constexpr T degrees() const
		{
			return _radians * Radians_To_Degrees;
		}

		/** Sets the current value in degrees. */
		constexpr void degrees(T value)
		{
			_radians = value * Degrees_To_Radians;
		}

		/////////////////////
		///   Operators   ///
	public:

		/** Converts this object to the underlying type. */
		constexpr operator T() const
		{
			return _radians;
		}

		/** Coversion operator for casting between precisions. */
		template <typename F>
		explicit operator TAngle<F>()
		{
			return{ static_cast<F>(_radians) };
		}

		//////////////////
		///   Fields   ///
	private:

		T _radians;
	};

	/** Alias for scalar angles. */
	using Angle = TAngle<Scalar>;

	/////////////////////
	///   Functions   ///

	/** Constructs an angle in radians. */
	template <typename T>
	constexpr TAngle<T> radians(T value)
	{
		return{ value };
	}

	/** Constructs an angle in degrees. */
	template <typename T>
	constexpr TAngle<T> degrees(T value)
	{
		return{ value * TAngle<T>::Degrees_To_Radians };
	}
}

SGE_REFLECT_TYPE_TEMPLATE(sge::TAngle, typename T);
