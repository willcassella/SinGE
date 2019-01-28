// CCharacterController.h
#pragma once

#include <Core/Math/Angle.h>
#include <Core/Math/Vec2.h>
#include "../../Component.h"

namespace sge
{
    struct SGE_ENGINE_API CCharacterController
    {
        SGE_REFLECTED_TYPE;
        struct SharedData;

        struct EJump
        {
            NodeId node;
        };

        struct EWalk
        {
            NodeId node;
            Vec2 direction;
        };

        struct ETurn
        {
            /* Amount to turn clockwise. */
            NodeId node;
            Angle amount;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        explicit CCharacterController(NodeId node, SharedData& shared_data);

        ///////////////////
        ///   Methods   ///
    public:

        static void register_type(Scene& scene);

        void to_archive(ArchiveWriter& writer) const;

        void from_archive(ArchiveReader& reader);

        NodeId node() const;

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

        void set_modified(const char* property_name);

        ////////////////
        ///   Data   ///
    private:

        float _step_height = 0.1f;
        Angle _max_slope = degrees(30);
        float _jump_speed = 10.f;
        float _fall_speed = 55.f;
        bool _on_ground = true;
        NodeId _node;
        SharedData* _shared_data = nullptr;
    };
}
