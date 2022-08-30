#pragma once
#include "net/Player.h"
#include <unordered_set>

class EventBus;
typedef struct _ENetHost ENetHost; // have to forward declare it like this because C sucks
typedef struct _ENetPeer ENetPeer;

namespace Net
{
  struct Address;

  class NetworkClient
  {
  public:
    NetworkClient(EventBus* eventBus);
    ~NetworkClient();

    NetworkClient(const NetworkClient&) = delete;
    NetworkClient(NetworkClient&&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;
    NetworkClient& operator=(NetworkClient&&) = delete;

    void ConnectToServer(const Address& address);
    void Disconnect();
    bool IsConnected() const;
    void SendSimpleMessage(const char* message);
    void Poll(double dt);

  private:
    EventBus* _eventBus;
    ENetHost* _client;
    ENetPeer* _server;
    //std::unordered_set<Net::Player> _players;
  };
}