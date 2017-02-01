// CCapsuleCollider.h
#pragma once

#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CCapsuleCollider final : public TComponentInterface<CCapsuleCollider>
    {
    public:

        SGE_REFLECTED_TYPE;
        struct Data;

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        void reset(Data& data);

        float radius() const;

        void radius(float value);

        float height() const;

        void height(float value);

        //////////////////
        ///   Fields   ///
    private:

        Data* _data = nullptr;
    };
}
