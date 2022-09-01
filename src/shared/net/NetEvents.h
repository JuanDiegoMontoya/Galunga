#pragma once
#include "utils/EventBus.h"

namespace Net
{
  struct Disconnect : public EventBus::Event {};
}