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

        struct SGE_ENGINE_API FTurnEvent
        {
            SGE_REFLECTED_TYPE;

            //////////////////
            ///   Fields   ///
        public:

            /* Amount to turn clockwise. */
            Angle amount;
        };

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        void reset(Data& data);

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

        void jump() const;

        void walk(const Vec2& direction) const;

        void turn(Angle amount) const;

    private:

        void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags) override;

        ////////////////
        ///   Data   ///
    private:

        Data* _data = nullptr;
        mutable bool _current_jumped = false;
        mutable std::vector<EntityId> _ord_jumped;
        mutable std::vector<EntityId> _ord_walked_ents;
        mutable std::vector<FWalkEvent> _ord_walked_tags;
        mutable std::vector<EntityId> _ord_turned_ents;
        mutable std::vector<FTurnEvent> _ord_turned_tags;
    };
}
