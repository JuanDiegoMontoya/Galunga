#include "BulletSystem.h"
#include <shared/ecs/components/game/Bullet.h>
#include <shared/ecs/components/core/Transform.h>
#include <utils/EventBus.h>
#include <ecs/Entity.h>

#include <glm/gtx/matrix_transform_2d.hpp>

namespace shared::ecs
{
  BulletSystem::BulletSystem(Scene* scene, EventBus* eventBus)
    : System(scene, eventBus)
  {
  }

  void BulletSystem::Update(double dt)
  {
    for (auto&& [entity, transform, bullet] : SceneRegistry()->view<ecs::Transform, ecs::Bullet>().each())
    {
      transform.translation += bullet.velocity * float(dt);
      bullet.velocity = glm::mat2(glm::rotate(glm::mat3(1), float(3.14 * dt / 8.))) * bullet.velocity;
      transform.rotation += float(3.14 * dt / 8.);
    }
  }
}