#include "System.h"
#include <ecs/Scene.h>

namespace shared::ecs
{
  System::System(Scene* scene, EventBus* eventBus)
    : _scene(scene),
      _eventBus(eventBus)
  {
  }

  System::~System()
  {
  }

  entt::registry* System::SceneRegistry()
  {
    return _scene->_registry.get();
  }
}