#pragma once
#include <glm/vec2.hpp>

namespace shared::ecs
{
  struct Transform
  {
    glm::vec2 translation;
    float rotation; // radians
    glm::vec2 scale;
  };
}