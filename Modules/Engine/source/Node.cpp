// Node.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Node.h"
#include "../include/Engine/Scene.h"

SGE_REFLECT_TYPE(sge::Node);

namespace sge
{
    Node::Node()
        : _id(NULL_ID),
        _root(NULL_ID),
        _scene(nullptr),
        _world_matrix_dirty(false)
    {
    }

    Node::Id Node::get_id() const
    {
        return _id;
    }

    const std::string& Node::get_name() const
    {
        return _name;
    }

    Node::Id Node::get_root() const
    {
        return _root;
    }

    void Node::set_root(Node* root)
    {
        // If this node has a root
        if (_root != NULL_ID)
        {
            // Remove this from the root
            auto* old_root = _scene->get_node(_root);
            old_root->_child_nodes.erase(get_id());
        }

        // Add this as a child of the given root
        if (root)
        {
            root->_child_nodes.insert(get_id());
            _root = root->get_id();
        }
        else
        {
            _root = NULL_ID;
        }

        _world_matrix_dirty = true;

        // Notify components
        for (auto component : _components)
        {
            component->on_node_root_update(this);
        }
    }

    void Node::get_children(std::vector<Id>* out_children) const
    {
        out_children->assign(_child_nodes.size(), NULL_ID);
        auto* out = out_children->data();

        std::size_t i = 0;
        for (auto child : _child_nodes)
        {
            out[i] = child;
            ++i;
        }
    }

    void Node::add_child(Node& child)
    {
        child.set_root(this);
    }

    void Node::remove_child(Node& child)
    {
        // Only remove if the child exists in this node's children
        const auto iter = _child_nodes.find(child.get_id());
        if (iter == _child_nodes.end())
        {
            return;
        }

        _child_nodes.erase(iter);
        child._root = NULL_ID;
        child._world_matrix_dirty = true;

        // Notify child's components
        for (auto component : child._components)
        {
            component->on_node_root_update(&child);
        }
    }

    Vec3 Node::get_local_position() const
    {
        return _local_position;
    }

    void Node::set_local_position(Vec3 pos)
    {
        _local_position = pos;
        _world_matrix_dirty = true;
        transform_notify_components();
    }

    Vec3 Node::get_local_scale() const
    {
        return _local_scale;
    }

    void Node::set_local_scale(Vec3 scale)
    {
        _local_scale = scale;
        _world_matrix_dirty = true;
        transform_notify_components();
    }

    Quat Node::get_local_rotation() const
    {
        return _local_rotation;
    }

    void Node::set_local_rotation(Quat rot)
    {
        _local_rotation = rot;
        _world_matrix_dirty = true;
        transform_notify_components();
    }

    void Node::get_world_matrix(Mat4* out) const
    {
        if (!_world_matrix_dirty)
        {
            *out = _cached_world_matrix;
            return;
        }

        // Calculate the local matrix
        const auto local = Mat4::translation(_local_position) * Mat4::rotate(_local_rotation) * Mat4::scale(_local_scale);

        // Calculate the parent matrix
        if (_root != NULL_ID)
        {
            Mat4 parent_mat;
            _scene->get_node(_root)->get_world_matrix(&parent_mat);
            _cached_world_matrix = parent_mat * local;
            _world_matrix_dirty = false;
        }
        else
        {
            _cached_world_matrix = local;
            _world_matrix_dirty = false;
        }
    }

    void Node::transform_notify_components()
    {
        for (auto component : _components)
        {
            component->on_node_transform_update(this);
        }
    }
}
