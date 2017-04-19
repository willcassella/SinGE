// CSpotlight.h
#pragma once

#include <Resource/Misc/Color.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CSpotlight
	{
		SGE_REFLECTED_TYPE;

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

		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CSpotlight(NodeId node, SharedData& shared_data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		Shape shape() const;

		void shape(Shape shape);

		void set_cone(Angle angle);

		void set_frustum(Angle horiz_angle, Angle vert_angle);

		Angle cone_angle() const;

		void cone_angle(Angle value);

		Angle frustum_horiz_angle() const;

		void frustum_horiz_angle(Angle value);

		Angle frustum_vert_angle() const;

		void frustum_vert_angle(Angle value);

		float near_clipping_plane() const;

		void near_clipping_plane(float value);

		float far_clipping_plane() const;

		void far_clipping_plane(float value);

		color::RGBF32 intensity() const;

		void intensity(color::RGBF32 value);

		bool is_lightmask_volume() const;

		void is_lightmask_volume(bool value);

		uint32 lightmask_group() const;

		void lightmask_group(uint32 value);

	private:

		void set_modified(const char* property_name);

		//////////////////
		///   Fields   ///
	private:

		Shape _shape = Shape::CONE;
		Angle _cone_angle = degrees(30.f);
		Angle _frustum_horiz_angle = degrees(30.f);
		Angle _frustum_vert_angle = degrees(30.f);
		float _near_clipping_plane = 0.1f;
		float _far_clipping_plane = 1.0f;
		color::RGBF32 _intensity = color::RGBF32::white();
		bool _lightmask_volume = false;
		uint32 _lightmask_group = 0;
		NodeId _node;
		SharedData* _shared_data = nullptr;
	};
}

SGE_REFLECTED_ENUM(SGE_ENGINE_API, sge::CSpotlight::Shape);
