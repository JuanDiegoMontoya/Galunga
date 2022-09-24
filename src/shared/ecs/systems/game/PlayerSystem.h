#pragma once
#include <ecs/systems/System.h>
#include <ecs/events/game/PlayerEvents.h>

namespace shared::ecs
{
  class PlayerSystem : public System
  {
  public:
    PlayerSystem(Scene* scene, EventBus* eventBus);

    void Update(double dt) override;

  private:
    static constexpr float MAX_SPEED = 2;
    float horizontalMovement = 0;
    float verticalMovement = 0;

    void HandleHorizontalMovement(PlayerMoveHorizontal& e);
    void HandleVerticalMovement(PlayerMoveVertical& e);
    void HandleShoot(PlayerShoot& e);
  };
}