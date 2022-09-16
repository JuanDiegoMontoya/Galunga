#pragma once

namespace shared::ecs
{
  struct Lifetime
  {
    int microsecondsLeft;
  };

  struct DeleteNextTick
  {
    char _dummy;
  };

  struct DeleteInNTicks
  {
    int ticks;
  };
}