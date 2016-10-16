// CTransform3D.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CTransform3D
	{
		SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		CTransform3D();

		///////////////////
		///   Methods   ///
	public:

		static bool has_parent(TComponentInstance<const CTransform3D> self);

		static TComponentId<CTransform3D> get_parent(TComponentInstance<const CTransform3D> self);

		static void set_parent(TComponentInstance<CTransform3D> self, Frame& frame, TComponentInstance<const CTransform3D> parent);

		static Vec3 get_local_position(TComponentInstance<const CTransform3D> self);

		static void set_local_position(TComponentInstance<CTransform3D> self, Frame& frame, Vec3 pos);

		static Vec3 get_local_scale(TComponentInstance<const CTransform3D> self);

		static void set_local_scale(TComponentInstance<CTransform3D> self, Frame& frame, Vec3 scale);

		static Quat get_local_rotation(TComponentInstance<const CTransform3D> self);

		static void set_local_rotation(TComponentInstance<CTransform3D> self, Frame& frame, Quat rot);

		static Mat4 get_local_matrix(TComponentInstance<const CTransform3D> self);

		static Vec3 get_world_position(TComponentInstance<const CTransform3D> self, const Frame& frame);

		static Vec3 get_world_scale(TComponentInstance<const CTransform3D> self, const Frame& frame);

		static Quat get_world_rotation(TComponentInstance<const CTransform3D> self, const Frame& frame);

		static Mat4 get_world_matrix(TComponentInstance<const CTransform3D> self, const Frame& frame);

	private:

		static Mat4 get_parent_matrix(TComponentInstance<const CTransform3D> self, const Frame& frame);

		//////////////////
		///   Fields   ///
	private:

		Vec3 _local_position;
		Vec3 _local_scale;
		Quat _local_rotation;
		TComponentId<CTransform3D> _parent;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API TTransformChanged
		{
			SGE_REFLECTED_TYPE;
		};

		struct SGE_ENGINE_API TParentChanged
		{
			SGE_REFLECTED_TYPE;
		};
	};
}
