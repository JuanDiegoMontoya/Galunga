#pragma once
#include <ecs/events/AxisBindingBase.h>

namespace shared::ecs
{
  struct PlayerMoveHorizontal : AxisBindingBase {};
  struct PlayerMoveVertical : AxisBindingBase {};
  struct PlayerShoot : AxisBindingBase {};
}