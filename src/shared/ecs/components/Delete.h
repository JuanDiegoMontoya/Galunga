#pragma once

namespace ecs
{
  struct DeleteNextFrame
  {
    char _dummy;
  };

  struct DeleteInNTicks
  {
    int ticks;
  };
}