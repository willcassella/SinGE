// Node.h
#pragma once

#include <Core/Reflection/Reflection.h>
#include <Core/Math/Mat4.h>
#include "config.h"

namespace sge
{
    class SGE_ENGINE_API Node
    {
        SGE_REFLECTED_TYPE;
        using Id = uint64_t;

        ///////////////////
        ///   Methods   ///
    public:

        Id get_id() const
        {
            return _id;
        }

        virtual const std::string& get_name() const = 0;

        virtual const Node* get_root() const = 0;

        virtual Node* get_root() = 0;

        virtual Mat4 get_world_transform() const = 0;

        //////////////////
        ///   Fields   ///
    private:

        Id _id = 0;
    };
}
