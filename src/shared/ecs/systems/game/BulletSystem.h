#pragma once
#include <ecs/systems/System.h>

namespace shared::ecs
{
  class BulletSystem : public System
  {
  public:
    BulletSystem(Scene* scene, EventBus* eventBus);

    void Update(double dt) override;

  private:
  };
}