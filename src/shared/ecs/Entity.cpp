#include "Entity.h"
#include "ecs/components/core/Lifetime.h"

namespace shared::ecs
{
  void Entity::Destroy()
  {
    G_ASSERT_MSG(*this, "Tried to delete invalid entity");

    GetOrAddComponent<ecs::DeleteNextTick>();
  }
}