#pragma once

namespace shared::ecs
{
  struct Lifetime
  {
    int microsecondsLeft;
  };

  struct DeleteNextTick {};

  struct DeleteInNTicks
  {
    int ticks;
  };
}