#pragma once
#include <cstdint>
#include <glm/vec4.hpp>

namespace shared::ecs
{
  struct Sprite
  {
    std::uint32_t index;
    glm::u8vec4 tint = { 255, 255, 255, 255 };
  };
}