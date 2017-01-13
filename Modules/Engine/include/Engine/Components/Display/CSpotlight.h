// CSpotlight.h
#pragma once

#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CSpotlight final : TComponentInterface<CSpotlight>
	{
		enum class Shape
		{
			/**
			 * \brief This spotlight is in the shape of a cone.
			 */
			CONE,

			/**
			 * \brief This spotlight is in the shape of a frustum.
			 */
			FRUSTUM
		};

		struct Data;
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CSpotlight(ProcessingFrame& pframe, EntityId entity, Data& data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		Shape shape() const;

		void shape(Shape shape);

		void set_cone(float radius);

		void set_frustum(float width, float height);

		float distance() const;

		void distance(float value);

		float intensity() const;

		void intensity(float value);

		float cone_radius() const;

		void cone_radius(float value);

		float frustum_width() const;

		void frustum_width(float value);

		float frustum_height() const;

		void frustum_height(float value);

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
	};
}
