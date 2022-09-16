#pragma once
#include <unordered_set>

class EventBus;
typedef struct _ENetHost ENetHost; // have to forward declare it like this because C sucks
typedef struct _ENetPeer ENetPeer;

namespace server::net
{
  class Host
  {
  public:
    Host(EventBus* eventBus);
    ~Host();

    Host(const Host&) = delete;
    Host(Host&&) = delete;
    Host& operator=(const Host&) = delete;
    Host& operator=(Host&&) = delete;

    void Poll(double dt);

  private:
    EventBus* _eventBus;
    ENetHost* _server;
    std::unordered_set<ENetPeer*> _clients;
  };
}
