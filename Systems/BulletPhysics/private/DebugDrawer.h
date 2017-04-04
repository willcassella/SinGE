// DebugDrawer.h
#pragma once

#include <Core/Math/Vec3.h>
#include <Engine/Util/DebugDraw.h>
#include <btBulletCollisionCommon.h>

namespace sge
{
    namespace bullet_physics
    {
        class DebugDrawer final : public btIDebugDraw
        {
            ///////////////////
            ///   Methods   ///
        public:

            void drawLine(
                const btVector3& from,
                const btVector3& to,
                const btVector3& color) override;

            void drawContactPoint(
                const btVector3& pointOnB,
                const btVector3& normalOnB,
                btScalar distance,
                int lifeTime,
                const btVector3& color) override;

            void reportErrorWarning(
                const char* warningString) override;

            void draw3dText(
                const btVector3& location,
                const char* textString) override;

            void setDebugMode(
                int debugMode) override;

            int	getDebugMode() const override;

            //////////////////
            ///   Fields   ///
        public:

            std::vector<DebugLine> lines;
        };
    }
}
