// CTransform3D.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Components/CTransform3D.h"
#include "../../include/Engine/Scene.h"

namespace sge
{
	SGE_REFLECT_TYPE(sge::CTransform3D)
	.property("local_position", component_getter(CTransform3D::get_local_position), component_setter(CTransform3D::set_local_position))
	.property("local_scale", component_getter(CTransform3D::get_local_scale), component_setter(CTransform3D::set_local_scale))
	.property("local_rotation", component_getter(CTransform3D::get_local_rotation), component_setter(CTransform3D::set_local_rotation))
	.property("world_position", component_getter(CTransform3D::get_world_position), nullptr)
	.property("world_scale", component_getter(CTransform3D::get_world_scale), nullptr)
	.property("world_rotation", component_getter(CTransform3D::get_world_rotation), nullptr)
	.field("local_position", &CTransform3D::_local_position)
	.field("local_scale", &CTransform3D::_local_scale)
	.field("local_rotation", &CTransform3D::_local_rotation);

	SGE_REFLECT_TYPE(sge::CTransform3D::TTransformChanged);
	SGE_REFLECT_TYPE(sge::CTransform3D::TParentChanged);

	CTransform3D::CTransform3D()
	{
		_local_scale = { 1, 1, 1 };
	}

	bool CTransform3D::has_parent(TComponentInstance<const CTransform3D> self)
	{
		return !self->_parent.is_null();
	}

	TComponentId<CTransform3D> CTransform3D::get_parent(TComponentInstance<const CTransform3D> self)
	{
		return self->_parent;
	}

	void CTransform3D::set_parent(TComponentInstance<CTransform3D> self, Frame& frame, TComponentInstance<const CTransform3D> parent)
	{
		self->_parent = parent.id();
	}

	Vec3 CTransform3D::get_local_position(TComponentInstance<const CTransform3D> self)
	{
		return self->_local_position;
	}

	void CTransform3D::set_local_position(TComponentInstance<CTransform3D> self, Frame& frame, Vec3 pos)
	{
		self->_local_position = pos;
	}

	Vec3 CTransform3D::get_local_scale(TComponentInstance<const CTransform3D> self)
	{
		return self->_local_scale;
	}

	void CTransform3D::set_local_scale(TComponentInstance<CTransform3D> self, Frame& frame, Vec3 scale)
	{
		self->_local_scale = scale;
	}

	Quat CTransform3D::get_local_rotation(TComponentInstance<const CTransform3D> self)
	{
		return self->_local_rotation;
	}

	void CTransform3D::set_local_rotation(TComponentInstance<CTransform3D> self, Frame& frame, Quat rot)
	{
		self->_local_rotation = rot;
	}

	Mat4 CTransform3D::get_local_matrix(TComponentInstance<const CTransform3D> self)
	{
		Mat4 result;
		result *= Mat4::scale(self->_local_scale);
		result *= Mat4::rotate(self->_local_rotation);
		result *= Mat4::translation(self->_local_position);

		return result;
	}

	Vec3 CTransform3D::get_world_position(TComponentInstance<const CTransform3D> self, const Frame& frame)
	{
		if (has_parent(self))
		{
			return get_local_position(self);
		}
		else
		{
			return get_parent_matrix(self, frame) * get_local_position(self);
		}
	}

	Vec3 CTransform3D::get_world_scale(TComponentInstance<const CTransform3D> self, const Frame& frame)
	{
		if (has_parent(self))
		{
			return get_local_scale(self);
		}
		else
		{
			return get_parent_matrix(self, frame) * get_local_scale(self);
		}
	}

	Quat CTransform3D::get_world_rotation(TComponentInstance<const CTransform3D> self, const Frame& frame)
	{
		if (has_parent(self))
		{
			return get_local_rotation(self);
		}
		else
		{
			return /*get_parent_matrix(self, scene) **/ get_local_rotation(self); // TODO
		}
	}

	Mat4 CTransform3D::get_world_matrix(TComponentInstance<const CTransform3D> self, const Frame& frame)
	{
		return get_parent_matrix(self, frame) * get_local_matrix(self);
	}

	Mat4 CTransform3D::get_parent_matrix(TComponentInstance<const CTransform3D> self, const Frame& frame)
	{
		if (has_parent(self))
		{
			auto parent = frame.scene().get_component(self->_parent);
			return get_parent_matrix(parent, frame);
		}
		else
		{
			return Mat4{};
		}
	}
}
