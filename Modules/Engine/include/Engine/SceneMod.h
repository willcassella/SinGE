// SceneMod.h
#pragma once

#include <Core/Math/Vec3.h>
#include <Core/Math/Quat.h>

namespace sge
{
    struct Node;

    struct NodeLocalTransformMod
    {
        Node* node;
        Vec3 local_pos;
        Vec3 local_scale;
        Quat local_rot;
    };

    struct NodeRootMod
    {
        Node* node;
        Node* root;
    };
}
