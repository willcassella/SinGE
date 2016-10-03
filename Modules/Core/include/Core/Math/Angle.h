// Angle.h
#pragma once

#include "../Reflection/Reflection.h"

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

	namespace specialized
	{
		template <typename T>
		struct GetType< TAngle<T> >
		{
			static const NativeTypeInfo type_info;

			static const NativeTypeInfo& get_type()
			{
				return type_info;
			}

			static const NativeTypeInfo& get_type(const TAngle<T>& /*value*/)
			{
				return type_info;
			}
		};

		template <typename T>
		const NativeTypeInfo GetType<TAngle<T>>::type_info = NativeTypeInfoBuilder<TAngle<T>>("TAngle");
	}
}
