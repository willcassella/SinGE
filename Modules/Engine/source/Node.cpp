// Node.cpp

#include <algorithm>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Node.h"
#include "../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::Node);

namespace sge
{
    Node::Node()
        : _scene(nullptr),
		_hierarchy_depth(0),
        _mod_state(NONE),
		_transform_mod_index(-1),
		_root_mod_index(-1),
        _local_scale(1.f, 1.f, 1.f)
    {
    }

    NodeId Node::get_id() const
    {
        return _id;
    }

    const std::string& Node::get_name() const
    {
        return _name;
    }

    Node::ModState_t Node::get_mod_state() const
    {
        return _mod_state;
    }

    NodeId Node::get_root() const
    {
        return _root;
    }

    void Node::set_root(Node* root)
    {
		auto& root_mod = get_root_mod();
		set_mod_state(_mod_state | ROOT_PENDING);
		root_mod.root = root;
    }

	NodeId Node::get_pending_root() const
    {
		if (_root_mod_index == -1)
		{
			return _root;
		}

		const auto* const root = _scene->get_raw_scene_data().node_root_changes[_root_mod_index].root;
		return root ? root->get_id() : NodeId::null_id();
    }

	std::size_t Node::get_num_children() const
    {
		return _child_nodes.size();
    }

    std::size_t Node::get_children(
		std::size_t start_index,
		std::size_t num_children,
		std::size_t* out_num_children,
		NodeId* out_children) const
    {
		if (_child_nodes.size() <= start_index)
		{
			*out_num_children = 0;
			return 0;
		}

		const auto num_copy = std::min(num_children, _child_nodes.size() - start_index);
		std::memcpy(out_children, _child_nodes.data() + start_index, num_copy * sizeof(NodeId));
		*out_num_children = num_copy;
		return num_copy;
    }

    void Node::add_child(Node& child)
    {
        child.set_root(this);
    }

    void Node::remove_child(Node& child)
    {
        // Only remove if the child exists in this node's children
		const auto iter = std::find(_child_nodes.begin(), _child_nodes.end(), child.get_id());
        if (iter == _child_nodes.end())
        {
            return;
        }

		child.set_root(nullptr);
    }

    Vec3 Node::get_local_position() const
    {
        return _local_position;
    }

    void Node::set_local_position(Vec3 pos)
    {
		auto& trans_mod = get_transform_mod();
		set_mod_state(_mod_state | TRANSFORM_PENDING);
		trans_mod.local_pos = pos;
    }

	Vec3 Node::get_pending_local_position() const
	{
		if (_transform_mod_index == -1)
		{
			return _local_position;
		}

		const auto& trans_mod = _scene->get_raw_scene_data().node_transform_changes[_transform_mod_index];
		return trans_mod.local_pos;
	}

	Vec3 Node::get_local_scale() const
    {
        return _local_scale;
    }

    void Node::set_local_scale(Vec3 scale)
    {
		auto& trans_mod = get_transform_mod();
		set_mod_state(_mod_state | TRANSFORM_PENDING);
		trans_mod.local_scale = scale;
    }

	Vec3 Node::get_pending_local_scale() const
    {
	    if (_transform_mod_index == -1)
	    {
			return _local_scale;
	    }

		const auto& trans_mod = _scene->get_raw_scene_data().node_transform_changes[_transform_mod_index];
		return trans_mod.local_scale;
    }

    Quat Node::get_local_rotation() const
    {
        return _local_rotation;
    }

    void Node::set_local_rotation(Quat rot)
    {
		auto& trans_mod = get_transform_mod();
		set_mod_state(_mod_state | TRANSFORM_PENDING);
		trans_mod.local_rot = rot;
    }

	Quat Node::get_pending_local_rotation() const
	{
		if (_transform_mod_index == -1)
		{
			return _local_rotation;
		}

		const auto& trans_mod = _scene->get_raw_scene_data().node_transform_changes[_transform_mod_index];
		return trans_mod.local_rot;
	}

	Mat4 Node::get_world_matrix() const
    {
		return _cached_world_matrix;
    }

	NodeTransformMod& Node::get_transform_mod()
	{
		if (_transform_mod_index != -1)
		{
			return _scene->get_raw_scene_data().node_transform_changes[_transform_mod_index];
		}

		NodeTransformMod trans_mod;
		trans_mod.node = this;
		trans_mod.local_pos = _local_position;
		trans_mod.local_scale = _local_scale;
		trans_mod.local_rot = _local_rotation;

		auto& transform_mod_array = _scene->get_raw_scene_data().node_transform_changes;
		const auto index = transform_mod_array.size();
    	_transform_mod_index = static_cast<int32>(index);
		transform_mod_array.push_back(trans_mod);
		return transform_mod_array[index];
	}

	NodeRootMod& Node::get_root_mod()
	{
		if (_root_mod_index != -1)
		{
			return _scene->get_raw_scene_data().node_root_changes[_root_mod_index];
		}

		NodeRootMod root_mod;
		root_mod.node = this;
		root_mod.root = nullptr;

		auto& root_mod_array = _scene->get_raw_scene_data().node_root_changes;
		const auto index = root_mod_array.size();
		_root_mod_index = static_cast<int32>(index);
		root_mod_array.push_back(root_mod);
		return root_mod_array[index];
	}

	void Node::set_mod_state(ModState_t state)
	{
		if (state != NONE && _mod_state == NONE)
		{
			_scene->get_raw_scene_data().modified_nodes.push_back(this);
		}

		_mod_state = state;
	}
}
