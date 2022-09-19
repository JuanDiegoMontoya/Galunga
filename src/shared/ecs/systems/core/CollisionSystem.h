#pragma once
#include <ecs/systems/System.h>

namespace shared::ecs
{
  class CollisionSystem : public System
  {
  public:
    CollisionSystem(Scene* scene, EventBus* eventBus);

    void Update(double dt) override;

  private:

  };
}