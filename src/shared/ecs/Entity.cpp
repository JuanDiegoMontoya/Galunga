#include "Entity.h"
#include "ecs/components/Delete.h"

namespace ecs
{
  void Entity::Destroy()
  {
    G_ASSERT_MSG(*this, "Tried to delete invalid entity");

    GetOrAddComponent<ecs::DeleteNextFrame>();
  }
}