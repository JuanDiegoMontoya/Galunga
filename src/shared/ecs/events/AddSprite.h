#pragma once
#include <utils/EventBus.h>
#include <string>

namespace ecs
{
  struct AddSprite : public EventBus::Event
  {
    std::string path;
  };
}