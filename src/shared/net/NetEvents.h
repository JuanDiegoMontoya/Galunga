#pragma once
#include "utils/EventBus.h"

namespace shared::net
{
  struct Disconnect : public EventBus::Event {};
}