#pragma once
#include <utils/EventBus.h>
#include <string>

namespace shared::ecs
{
  struct AddSprite : public EventBus::Event
  {
    std::string path;
  };
}