#pragma once
#include <glm/vec2.hpp>

namespace shared::ecs
{
  enum class BulletTeam
  {
    PLAYER,
    ENEMY
  };

  struct Bullet
  {
    glm::vec2 velocity;
    BulletTeam team;
  };
}