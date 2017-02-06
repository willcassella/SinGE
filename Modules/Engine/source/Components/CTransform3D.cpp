// CTransform3D.cpp

#include <memory>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../../include/Engine/Components/CTransform3D.h"
#include "../../include/Engine/ProcessingFrame.h"
#include "../../include/Engine/Scene.h"
#include "../../include/Engine/Util/MapComponentContainer.h"
#include "../../include/Engine/TagBuffer.h"

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

    static Mat4 generate_matrix(const Vec3& pos, const Vec3& scale, const Quat& rot)
    {
        Mat4 result;
        result *= Mat4::translation(pos);
        result *= Mat4::rotate(rot);
        result *= Mat4::scale(scale);

        return result;
    }

    static Mat4 generate_world_matrix(const CTransform3D::Data& data, const std::map<EntityId, CTransform3D::Data>& data_map)
    {
        // Calculate this entity's matrix
        auto mat = generate_matrix(data.local_position, data.local_scale, data.local_rotation);

        if (data.parent != WORLD_ENTITY)
        {
            return generate_world_matrix(data_map.find(data.parent)->second, data_map) * mat;
        }
        else
        {
            return mat;
        }
    }

	SGE_REFLECT_TYPE(sge::CTransform3D)
	.property("local_position", &CTransform3D::get_local_position, &CTransform3D::set_local_position)
	.property("local_scale", &CTransform3D::get_local_scale, &CTransform3D::set_local_scale)
	.property("local_rotation", &CTransform3D::get_local_rotation, &CTransform3D::set_local_rotation)
	.property("world_position", &CTransform3D::get_world_position, nullptr)
	.property("world_scale", &CTransform3D::get_world_scale, nullptr)
	.property("world_rotation", &CTransform3D::get_world_rotation, nullptr);

	SGE_REFLECT_TYPE(sge::CTransform3D::FParentChanged);

	void CTransform3D::register_type(Scene& scene)
	{
		scene.register_component_type(type_info, std::make_unique<MapComponentContainer<CTransform3D, Data>>());
	}

    void CTransform3D::reset(Data& data, std::map<EntityId, Data>& cont_data)
    {
        _data = &data;
        _data_map = &cont_data;
    }

    bool CTransform3D::has_parent() const
	{
		return _data->parent != WORLD_ENTITY;
	}

	TComponentId<CTransform3D> CTransform3D::get_parent() const
	{
		return _data->parent;
	}

	void CTransform3D::set_parent(TComponentId<CTransform3D> parent)
	{
        if (_data->parent == parent.entity())
        {
            return;
        }

        // TODO: Check for parent cycle
		_data->parent = parent.entity();
        _parent_changed_tags.add_single_tag(entity(), FParentChanged{});
	}

    std::vector<EntityId> CTransform3D::get_children() const
    {
        // I know this is literally HORRIBLE, but I don't have time to change it.
	    // If it becomes a performance problem, I can fix it in the future
        std::vector<EntityId> result;

        for (const auto& elem : *_data_map)
        {
            if (elem.second.parent == entity())
            {
                result.push_back(elem.first);
            }
        }

        return result;
    }

    Vec3 CTransform3D::get_local_position() const
	{
		return _data->local_position;
	}

	void CTransform3D::set_local_position(Vec3 pos)
	{
        checked_setter(pos, _data->local_position);
	}

	Vec3 CTransform3D::get_local_scale() const
	{
		return _data->local_scale;
	}

	void CTransform3D::set_local_scale(Vec3 scale)
	{
        checked_setter(scale, _data->local_scale);
	}

	Quat CTransform3D::get_local_rotation() const
	{
		return _data->local_rotation;
	}

	void CTransform3D::set_local_rotation(Quat rot)
	{
        checked_setter(rot, _data->local_rotation);
	}

	Mat4 CTransform3D::get_local_matrix() const
	{
        return generate_matrix(_data->local_position, _data->local_scale, _data->local_rotation);
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
			return /*get_parent_matrix(self, scene) **/ get_local_rotation();
		}
	}

	Mat4 CTransform3D::get_world_matrix() const
	{
		return get_parent_matrix() * get_local_matrix();
	}

    void CTransform3D::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        // Call base implementation
        ComponentInterface::generate_tags(tags);

        // Create tags
        _parent_changed_tags.create_buffer(type_info, tags);
    }

    Mat4 CTransform3D::get_parent_matrix() const
	{
		if (!has_parent())
		{
            return Mat4{};
		}

        return generate_world_matrix(_data_map->find(_data->parent)->second, *_data_map);
	}
}
