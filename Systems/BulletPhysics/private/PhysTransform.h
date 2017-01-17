// PhysTransform.h
#pragma once

#include <btBulletDynamicsCommon.h>

namespace sge
{
    namespace bullet_physics
    {
        class PhsTransform final : public btMotionState
        {
            ///////////////////
            ///   Methods   ///
        public:

            void getWorldTransform(btTransform& worldTrans) const override
            {
                worldTrans.setOrigin(_position);
                worldTrans.setRotation(_rotation);
            }

            void setWorldTransform(const btTransform& worldTrans) override
            {
                _position = worldTrans.getOrigin();
                _rotation = worldTrans.getRotation();
            }

            //////////////////
            ///   Fields   ///
        private:

            btVector3 _position;
            btQuaternion _rotation;
        };
    }
}
