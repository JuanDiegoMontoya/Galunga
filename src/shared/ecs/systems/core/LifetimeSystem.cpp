#include "LifetimeSystem.h"
#include <ecs/components/core/Lifetime.h>
#include <ecs/Scene.h>
#include <entt/entity/registry.hpp>

namespace ecs
{
  LifetimeSystem::LifetimeSystem(Scene* scene, EventBus* eventBus)
    : System(scene, eventBus)
  {
  }

  void LifetimeSystem::Update(double dt)
  {
    auto deleteNTicksView = SceneRegistry()->view<ecs::DeleteInNTicks>();
    deleteNTicksView.each([this](auto entity, ecs::DeleteInNTicks& d)
      {
        if (--d.ticks <= 0)
        {
          SceneRegistry()->emplace<ecs::DeleteNextTick>(entity);
        }
      });

    auto deleteView = SceneRegistry()->view<ecs::DeleteNextTick>();
    SceneRegistry()->destroy(deleteView.begin(), deleteView.end());

    auto lifetimeView = SceneRegistry()->view<ecs::Lifetime>();
    lifetimeView.each([this, dt](auto entity, ecs::Lifetime& lifetime)
      {
        lifetime.microsecondsLeft -= static_cast<int>(dt * 1'000'000.0 + 0.5);
        if (lifetime.microsecondsLeft <= 0)
        {
          SceneRegistry()->emplace<ecs::DeleteNextTick>(entity);
        }
      });
  }
}