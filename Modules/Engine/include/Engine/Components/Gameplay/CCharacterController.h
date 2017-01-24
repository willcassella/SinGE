// CCharacterController.h
#pragma once

#include <Core/Math/Angle.h>
#include <Core/Math/Vec2.h>
#include "../../Component.h"

namespace sge
{
    class SGE_ENGINE_API CCharacterController final : public TComponentInterface<CCharacterController>
    {
    public:

        SGE_REFLECTED_TYPE;
        struct Data;

        struct SGE_ENGINE_API FJumpEvent
        {
            SGE_REFLECTED_TYPE;
        };

        struct SGE_ENGINE_API FWalkEvent
        {
            SGE_REFLECTED_TYPE;

            //////////////////
            ///   Fields   ///
        public:

            Vec2 direction;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        CCharacterController(ProcessingFrame& pframe, EntityId entity, Data& data);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        bool on_ground() const;

        void set_on_ground(bool value);

        float step_height() const;

        void step_height(float value);

        Angle max_slope() const;

        void max_slope(Angle value);

        float jump_speed() const;

        void jump_speed(float value);

        float fall_speed() const;

        void fall_speed(float value);

        void jump();

        void walk(const Vec2& direction);

        ////////////////
        ///   Data   ///
    private:

        Data* _data;
    };
}