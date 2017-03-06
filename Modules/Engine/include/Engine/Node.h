// Node.h
#pragma once

#include <set>
#include <Core/Reflection/Reflection.h>
#include <Core/Math/Mat4.h>
#include "EventChannel.h"

namespace sge
{
    struct Scene;
    class ComponentContainer;

    struct SGE_ENGINE_API Node
    {
        SGE_REFLECTED_TYPE;
        using Id = uint64;
        friend Scene;
        static constexpr Id NULL_ID = 0;

        ////////////////////////
        ///   Constructors   ///
    private:

        Node();

        ///////////////////
        ///   Methods   ///
    public:

        Id get_id() const;

        const std::string& get_name() const;

        Id get_root() const;

        void set_root(Node* root);

        void get_children(std::vector<Id>* out_children) const;

        void add_child(Node& child);

        void remove_child(Node& child);

        Vec3 get_local_position() const;

        void set_local_position(Vec3 pos);

        Vec3 get_local_scale() const;

        void set_local_scale(Vec3 scale);

        Quat get_local_rotation() const;

        void set_local_rotation(Quat rot);

        void get_world_matrix(Mat4* out) const;

    private:

        void transform_notify_components();

        //////////////////
        ///   Fields   ///
    private:

        Id _id;
        Id _root;
        std::string _name;
        std::set<Id> _child_nodes;
        std::set<ComponentContainer*> _components;
        Scene* _scene;
        Vec3 _local_position;
        Vec3 _local_scale;
        Quat _local_rotation;
        mutable Mat4 _cached_world_matrix;
        mutable bool _world_matrix_dirty;
    };
}
