#pragma once
#include <utils/EventBus.h>
#include <ecs/Entity.h>

namespace shared::ecs
{
  struct Collision
  {
    Entity entity0;
    Entity entity1;
  };
}