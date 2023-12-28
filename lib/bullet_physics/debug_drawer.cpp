#include <iostream>

#include "lib/bullet_physics/debug_drawer.h"
#include "lib/bullet_physics/util.h"

namespace sge {
namespace bullet_physics {
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
  DebugLine line;
  line.world_start = from_bullet(from);
  line.world_end = from_bullet(to);
  line.color.red(color.x());
  line.color.green(color.y());
  line.color.blue(color.z());
  lines.push_back(line);
}

void DebugDrawer::drawContactPoint(
    const btVector3& /*pointOnB*/,
    const btVector3& /*normalOnB*/,
    btScalar /*distance*/,
    int /*lifeTime*/,
    const btVector3& /*color*/
) {}

void DebugDrawer::reportErrorWarning(const char* warning) {
  std::cout << "WARNING [Bullet]: " << warning << std::endl;
}

void DebugDrawer::draw3dText(const btVector3& /*location*/, const char* /*textString*/) {}

void DebugDrawer::setDebugMode(int /*debugMode*/) {}

int DebugDrawer::getDebugMode() const {
  return DBG_DrawWireframe | DBG_DrawAabb;
}
}  // namespace bullet_physics
}  // namespace sge
