// CSpotlight.h
#pragma once

#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CSpotlight
	{
		SGE_REFLECTED_TYPE;

		struct SharedData;
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

	private:

		void set_modified(const char* property_name);

		//////////////////
		///   Fields   ///
	private:

		Shape _shape = Shape::CONE;
		float _cone_radius = 1.f;
		float _frustum_width = 1.f;
		float _frustum_height = 1.f;
		float _distance = 1.f;
		float _intensity = 1.f;
		NodeId _node;
		SharedData* _shared_data = nullptr;
	};
}
