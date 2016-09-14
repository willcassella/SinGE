// Transform3D.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../config.h"
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

		static bool has_parent(ComponentInstance<const CTransform3D> self);

		static Handle<CTransform3D> get_parent(ComponentInstance<const CTransform3D> self);

		static void set_parent(ComponentInstance<CTransform3D> self, Scene& scene, Handle<CTransform3D> parent);

		static Vec3 get_local_position(ComponentInstance<const CTransform3D> self);
		
		static void set_local_position(ComponentInstance<CTransform3D> self, Scene& scene, Vec3 pos);

		static Vec3 get_local_scale(ComponentInstance<const CTransform3D> self);

		static void set_local_scale(ComponentInstance<CTransform3D> self, Scene& scene, Vec3 scale);

		static Quat get_local_rotation(ComponentInstance<const CTransform3D> self);

		static void set_local_rotation(ComponentInstance<CTransform3D> self, Scene& scene, Quat rot);

		static Mat4 get_local_matrix(ComponentInstance<const CTransform3D> self);

		static Vec3 get_world_position(ComponentInstance<const CTransform3D> self, const Scene& scene);

		static Vec3 get_world_scale(ComponentInstance<const CTransform3D> self, const Scene& scene);

		static Quat get_world_rotation(ComponentInstance<const CTransform3D> self, const Scene& scene);

		static Mat4 get_world_matrix(ComponentInstance<const CTransform3D> self, const Scene& scene);

	private:

		static Mat4 get_parent_matrix(ComponentInstance<const CTransform3D> self, const Scene& scene);

		////////////////
		///   Data   ///
	private:

		Vec3 _local_position;
		Vec3 _local_scale;
		Quat _local_rotation;
		Handle<CTransform3D> _parent;

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
