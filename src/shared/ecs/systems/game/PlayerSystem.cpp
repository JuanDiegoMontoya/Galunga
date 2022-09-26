#include "PlayerSystem.h"
#include <ecs/components/game/Player.h>
#include <ecs/components/core/Transform.h>
#include <ecs/components/core/Sprite.h>
#include <ecs/components/game/Bullet.h>
#include <ecs/components/core/Lifetime.h>
#include <ecs/Scene.h>
#include <utils/EventBus.h>
#include <ecs/Entity.h>
#include <entt/entity/registry.hpp>

namespace shared::ecs
{
  PlayerSystem::PlayerSystem(Scene* scene, EventBus* eventBus)
    : System(scene, eventBus)
  {
    _eventBus->Subscribe(this, &PlayerSystem::HandleHorizontalMovement);
    _eventBus->Subscribe(this, &PlayerSystem::HandleVerticalMovement);
    _eventBus->Subscribe(this, &PlayerSystem::HandleShoot);
  }

  void PlayerSystem::Update([[maybe_unused]] double dt)
  {
    timeSinceLastShot += float(dt);

    // move every player controlled by this client
    for (auto&& [entity, transform] : SceneRegistry()->view<ecs::Transform, ecs::Player>().each())
    {
      transform.translation.x += float(horizontalMovement * dt);
      transform.translation.y += float(verticalMovement * dt);

      if (isShooting && timeSinceLastShot > SHOOT_DELAY)
      {
        auto newEntity = _scene->CreateEntity("bullet");
        auto& bullet = newEntity.AddComponent<ecs::Bullet>();
        bullet.team = BulletTeam::PLAYER;
        bullet.velocity = { 0, 10 };
        auto& newTransform = newEntity.AddComponent<ecs::Transform>();
        newTransform.translation = transform.translation;
        newTransform.rotation = 3.1415f / 2.0f;
        newTransform.scale = { .5f, .25f };
        newEntity.AddComponent<ecs::Sprite>().index = 0;
        newEntity.AddComponent<ecs::Lifetime>().microsecondsLeft = 2'000'000;
      }
    }

    if (isShooting && timeSinceLastShot > SHOOT_DELAY)
    {
      timeSinceLastShot = 0;
    }

    horizontalMovement = 0;
    verticalMovement = 0;
    isShooting = false;
  }

  void PlayerSystem::HandleHorizontalMovement(PlayerMoveHorizontal& e)
  {
    horizontalMovement += e.magnitude;
  }

  void PlayerSystem::HandleVerticalMovement(PlayerMoveVertical& e)
  {
    verticalMovement += e.magnitude;
  }

  void PlayerSystem::HandleShoot(PlayerShoot&)
  {
    isShooting = true;
  }
}