// CLightColor.h
#pragma once

#include <Resource/Misc/Color.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CLightColor : TComponentInterface<CLightColor>
	{
		struct Data;
		using Color_t = color::RGBA8;
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CLightColor(ProcessingFrame& pframe, EntityId entity, Data& data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		/**
		 * \brief Gets the whole color value.
		 * \return The combined color component values.
		 */
		Color_t color() const;

		/**
		 * \brief Sets the whole color value.
		 * \param value The combined color component values to set.
		 */
		void color(Color_t value);

		/**
		 * \brief Gets the red component of the color.
		 * \return The 8-bit red component of the color.
		 */
		Color_t::Red_t red() const;

		/**
		 * \brief
		 * \param value
		 */
		void red(Color_t::Red_t value);

		Color_t::Green_t green() const;

		void green(Color_t::Green_t value);

		Color_t::Blue_t blue() const;

		void blue(Color_t::Blue_t value);

		Color_t::Alpha_t alpha() const;

		void alpha(Color_t::Alpha_t value);

	private:

		void update_data();

		//////////////////
		///   Fields   ///
	private:

		Color_t _cached_color;
		Data* _data;
	};
}
