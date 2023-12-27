#pragma once

#include <btBulletCollisionCommon.h>

#include "lib/base/math/vec3.h"
#include "lib/engine/util/debug_draw.h"

namespace sge
{
    namespace bullet_physics
    {
        class DebugDrawer final : public btIDebugDraw
        {
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

            int getDebugMode() const override;

            std::vector<DebugLine> lines;
        };
    }
}
