#include "PlayerSystem.h"
#include <ecs/components/game/Player.h>
#include <ecs/components/core/Transform.h>
#include <ecs/components/core/Sprite.h>
#include <ecs/components/game/Bullet.h>
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
    // move every player controlled by this client
    for (auto&& [entity, transform] : SceneRegistry()->view<ecs::Transform, ecs::Player>().each())
    {
      transform.translation.x += float(horizontalMovement * dt);
      transform.translation.y += float(verticalMovement * dt);
    }

    horizontalMovement = 0;
    verticalMovement = 0;
  }

  void PlayerSystem::HandleHorizontalMovement(PlayerMoveHorizontal& e)
  {
    horizontalMovement = e.magnitude;
  }

  void PlayerSystem::HandleVerticalMovement(PlayerMoveVertical& e)
  {
    verticalMovement = e.magnitude;
  }

  void PlayerSystem::HandleShoot(PlayerShoot&)
  {
    // every player controlled by this client shoots
    for (auto&& [entity, transform] : SceneRegistry()->view<ecs::Transform, ecs::Player>().each())
    {
      for (int i = 0; i < 100; i++)
      {
        auto newEntity = _scene->CreateEntity("bullet");
        auto& bullet = newEntity.AddComponent<ecs::Bullet>();
        bullet.team = BulletTeam::PLAYER;
        bullet.velocity = { 0, 5 };
        auto& newTransform = newEntity.AddComponent<ecs::Transform>();
        newTransform.translation = transform.translation + glm::vec2{ i / 8.0f - 6.25f, 0 };
        newTransform.rotation = 3.1415f / 2.0f;
        newTransform.scale = { .15f, .125f };
        newEntity.AddComponent<ecs::Sprite>().index = 0;
      }
    }
  }
}