// CTransform3D.cpp

#include <memory>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Components/CTransform3D.h"
#include "../../include/Engine/ProcessingFrame.h"
#include "../../include/Engine/Scene.h"
#include "../../include/Engine/Util/BasicComponentContainer.h"

namespace sge
{
	struct CTransform3D::Data
	{
		///////////////////
		///   Methods   ///
	public:

		void to_archive(ArchiveWriter& writer) const
		{
			writer.object_member("local_position", local_position);
			writer.object_member("local_scale", local_scale);
			writer.object_member("local_rotation", local_rotation);
			writer.object_member("parent", parent);
		}

		void from_archive(ArchiveReader& reader)
		{
			reader.object_member("local_position", local_position);
			reader.object_member("local_scale", local_scale);
			reader.object_member("local_rotation", local_rotation);
			reader.object_member("parent", parent);
		}

		//////////////////
		///   Fields   ///
	public:

		Vec3 local_position = Vec3::zero();
		Vec3 local_scale = Vec3{ 1.f, 1.f, 1.f };
		Quat local_rotation = Quat{};
		EntityId parent = WORLD_ENTITY;
	};

	SGE_REFLECT_TYPE(sge::CTransform3D)
	.property("local_position", &CTransform3D::get_local_position, &CTransform3D::set_local_position)
	.property("local_scale", &CTransform3D::get_local_scale, &CTransform3D::set_local_scale)
	.property("local_rotation", &CTransform3D::get_local_rotation, &CTransform3D::set_local_rotation)
	.property("world_position", &CTransform3D::get_world_position, nullptr)
	.property("world_scale", &CTransform3D::get_world_scale, nullptr)
	.property("world_rotation", &CTransform3D::get_world_rotation, nullptr);

	SGE_REFLECT_TYPE(sge::CTransform3D::FTransformChanged);
	SGE_REFLECT_TYPE(sge::CTransform3D::FParentChanged);

	CTransform3D::CTransform3D(ProcessingFrame& pframe, EntityId entity, Data& data)
		: TComponentInterface<sge::CTransform3D>(pframe, entity),
		_data(&data)
	{
		_applied_parent_changed_tag = false;
		_applied_transform_changed_tag = false;
	}

	void CTransform3D::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<BasicComponentContainer<CTransform3D, Data>>());
	}

	bool CTransform3D::has_parent() const
	{
		return _data->parent != WORLD_ENTITY;
	}

	TComponentId<CTransform3D> CTransform3D::get_parent() const
	{
		return _data->parent;
	}

	void CTransform3D::set_parent(const CTransform3D& parent)
	{
		_data->parent = parent.entity();
		if (!_applied_parent_changed_tag)
		{
			processing_frame().create_tag(this->id(), FParentChanged{});
			_applied_parent_changed_tag = true;
		}
	}

	Vec3 CTransform3D::get_local_position() const
	{
		return _data->local_position;
	}

	void CTransform3D::set_local_position(Vec3 pos)
	{
		_data->local_position = pos;
		apply_transform_changed_tag();
	}

	Vec3 CTransform3D::get_local_scale() const
	{
		return _data->local_scale;
	}

	void CTransform3D::set_local_scale(Vec3 scale)
	{
		_data->local_scale = scale;
		apply_transform_changed_tag();
	}

	Quat CTransform3D::get_local_rotation() const
	{
		return _data->local_rotation;
	}

	void CTransform3D::set_local_rotation(Quat rot)
	{
		_data->local_rotation = rot;
		apply_transform_changed_tag();
	}

	Mat4 CTransform3D::get_local_matrix() const
	{
		Mat4 result;
		result *= Mat4::translation(_data->local_position);
		result *= Mat4::rotate(_data->local_rotation);
		result *= Mat4::scale(_data->local_scale);

		return result;
	}

	Vec3 CTransform3D::get_world_position() const
	{
		if (!this->has_parent())
		{
			return get_local_position();
		}
		else
		{
			return get_parent_matrix() * get_local_position();
		}
	}

	Vec3 CTransform3D::get_world_scale() const
	{
		if (has_parent())
		{
			return get_local_scale();
		}
		else
		{
			return get_parent_matrix() * get_local_scale();
		}
	}

	Quat CTransform3D::get_world_rotation() const
	{
		if (has_parent())
		{
			return get_local_rotation();
		}
		else
		{
			return /*get_parent_matrix(self, scene) **/ get_local_rotation(); // TODO
		}
	}

	Mat4 CTransform3D::get_world_matrix() const
	{
		return get_parent_matrix() * get_local_matrix();
	}

	Mat4 CTransform3D::get_parent_matrix() const
	{
		if (has_parent())
		{
			//auto parent = frame.scene().get_component(self->_parent);
			//return get_parent_matrix(parent, frame);
		}
		//else
		{
			return Mat4{};
		}
	}

	void CTransform3D::apply_transform_changed_tag()
	{
		if (!_applied_transform_changed_tag)
		{
			processing_frame().create_tag(this->id(), FTransformChanged{});
			_applied_transform_changed_tag = true;
		}
	}
}
