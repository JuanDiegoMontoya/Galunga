#pragma once
#include <glm/vec2.hpp>

namespace shared::ecs
{
  struct Collider
  {
    glm::vec2 scale = { 1, 1 };
  };
}