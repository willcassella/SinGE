#pragma once

#include "lib/base/math/vec3.h"
#include "lib/base/math/quat.h"

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
