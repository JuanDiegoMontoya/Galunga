#pragma once
#include "Scene.h"
#include "GAssert.h"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <utility>

namespace ecs
{
  class Entity
  {
  public:
    Entity() = default;
    Entity(entt::entity entityHandle, Scene* scene)
      : _entityHandle(entityHandle),
        _scene(scene)
    {
    }

    Entity(const Entity&) noexcept = default;

    [[nodiscard]] bool operator==(const Entity&) const = default;
    [[nodiscard]] bool operator!=(const Entity&) const = default;

    operator bool() const { return _entityHandle != entt::null; }
    operator entt::entity() { return _entityHandle; }

    void Destroy();

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
      G_ASSERT_MSG(!HasComponent<T>(), "Entity already has component");
      return _scene->_registry->emplace<T>(_entityHandle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& GetOrAddComponent(Args&&... args)
    {
      return _scene->_registry->get_or_emplace<T>(_entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    [[nodiscard]] T& GetComponent()
    {
      G_ASSERT_MSG(HasComponent<T>(), "Entity missing component");
      return _scene->_registry->get<T>(_entityHandle);
    }

    template<typename T>
    [[nodiscard]] T& GetComponent() const
    {
      G_ASSERT_MSG(HasComponent<T>(), "Entity missing component");
      return _scene->_registry->get<T>(_entityHandle);
    }

    template<typename T>
    [[nodiscard]] bool HasComponent() const
    {
      return _scene->_registry->try_get<T>(_entityHandle) != nullptr;
    }

  private:
    Scene* _scene = nullptr;
    entt::entity _entityHandle = entt::null;
  };
}