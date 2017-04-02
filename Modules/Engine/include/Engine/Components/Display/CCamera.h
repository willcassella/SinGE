// CCamera.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CPerspectiveCamera
	{
		SGE_REFLECTED_TYPE;
		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CPerspectiveCamera(NodeId node, SharedData& shared_data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

		float h_fov() const;

		void h_fov(float angle);

		float z_min() const;

		void z_min(float value);

		float z_max() const;

		void z_max(float value);

		Mat4 get_projection_matrix(float screen_ratio) const;

	private:

		void set_modified(const char* prop_name);

		//////////////////
		///   Fields   ///
	private:

		Angle _h_fov = degrees(90.f);
		float _z_min = 0.1f;
		float _z_max = 100.f;
		NodeId _node;
		SharedData* _shared_data = nullptr;
	};
}
