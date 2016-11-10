// CTransform3D.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CTransform3D : TComponentInterface<CTransform3D>
	{
		SGE_REFLECTED_TYPE;
		struct Data;

		////////////////////////
		///   Constructors   ///
	public:

		CTransform3D(ProcessingFrame& pframe, EntityId entity, Data& data);

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

		bool has_parent() const;

		TComponentId<CTransform3D> get_parent() const;

		void set_parent(const CTransform3D& parent);

		Vec3 get_local_position() const;

		void set_local_position(Vec3 pos);

		Vec3 get_local_scale() const;

		void set_local_scale(Vec3 scale);

		Quat get_local_rotation() const;

		void set_local_rotation(Quat rot);

		Mat4 get_local_matrix() const;

		Vec3 get_world_position() const;

		Vec3 get_world_scale() const;

		Quat get_world_rotation() const;

		Mat4 get_world_matrix() const;

	private:

		Mat4 get_parent_matrix() const;

		void apply_transform_changed_tag();

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
		bool _applied_transform_changed_tag;
		bool _applied_parent_changed_tag;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API FTransformChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API FParentChanged
		{
			SGE_REFLECTED_TYPE;
		};
	};
}
