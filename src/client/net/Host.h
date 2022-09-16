#pragma once
#include "net/Player.h"
#include <unordered_set>

class EventBus;
typedef struct _ENetHost ENetHost; // have to forward declare it like this because C sucks
typedef struct _ENetPeer ENetPeer;

namespace shared::net
{
  struct Address;
}

namespace client::net
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

    void ConnectToServer(const shared::net::Address& address);
    void Disconnect();
    bool IsConnected() const;
    void SendSimpleMessage(const char* message);
    void Poll(double dt);

  private:
    EventBus* _eventBus;
    ENetHost* _client;
    ENetPeer* _server;
    //std::unordered_set<net::Player> _players;
  };
}