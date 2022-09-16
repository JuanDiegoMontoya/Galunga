#include "LifetimeSystem.h"
#include <ecs/components/core/Lifetime.h>
#include <ecs/Scene.h>
#include <entt/entity/registry.hpp>

namespace shared::ecs
{
  LifetimeSystem::LifetimeSystem(Scene* scene, EventBus* eventBus)
    : System(scene, eventBus)
  {
  }

  void LifetimeSystem::Update(double dt)
  {
    auto deleteNTicksView = SceneRegistry()->view<DeleteInNTicks>();
    deleteNTicksView.each([this](auto entity, DeleteInNTicks& d)
      {
        if (--d.ticks <= 0)
        {
          SceneRegistry()->emplace<ecs::DeleteNextTick>(entity);
        }
      });

    auto deleteView = SceneRegistry()->view<DeleteNextTick>();
    SceneRegistry()->destroy(deleteView.begin(), deleteView.end());

    auto lifetimeView = SceneRegistry()->view<Lifetime>();
    lifetimeView.each([this, dt](auto entity, Lifetime& lifetime)
      {
        lifetime.microsecondsLeft -= static_cast<int>(dt * 1'000'000.0 + 0.5);
        if (lifetime.microsecondsLeft <= 0)
        {
          SceneRegistry()->emplace<ecs::DeleteNextTick>(entity);
        }
      });
  }
}