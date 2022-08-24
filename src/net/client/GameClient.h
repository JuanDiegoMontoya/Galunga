#pragma once

class EventBus;
struct ENetHost;

namespace Net
{
  class GameClient
  {
  public:
    GameClient(EventBus* eventBus);

  private:
    EventBus* _eventBus;
    ENetHost* _client;
  };
}