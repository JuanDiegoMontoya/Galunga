#include "net/client/NetworkClient.h"
#include "net/Address.h"
#include "net/NetEvents.h"
#include "GAssert.h"
#include "enet/enet.h"
#include <stdexcept>

namespace Net
{
  Net::NetworkClient::NetworkClient(EventBus* eventBus)
    : _eventBus(eventBus),
    _client(nullptr),
    _server(nullptr)
  {
    if (enet_initialize() != 0)
    {
      throw std::runtime_error("An error occurred while initializing ENet");
    }

    _client = enet_host_create(nullptr, // client
                               1, // # of outgoing connections
                               2, // max. channels
                               0, // max. incoming bandwidth
                               0);// max. outgoing bandwidth
    if (_client == nullptr)
    {
      throw std::runtime_error("An error occurred while creating the client");
    }
  }

  NetworkClient::~NetworkClient()
  {
    // we are still connected- try to disconnect
    if (_server)
    {
      enet_peer_disconnect_now(_server, 0);
    }
    enet_host_destroy(_client); // ENet allows the host to be null here
    enet_deinitialize();
  }

  void NetworkClient::ConnectToServer(const Address& address)
  {
    ENetAddress eaddress{};
    enet_address_set_host(&eaddress, address.host.c_str());
    eaddress.port = address.port;
    eaddress.sin6_scope_id = 0;
    _server = enet_host_connect(_client, &eaddress, 2, 0);

    if (_server == nullptr)
    {
      throw std::runtime_error("Failed to identify a server at that address");
    }

    ENetEvent event;
    if (enet_host_service(_client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT)
    {
      printf("Connection to %s:%hu succeeded\n", 
        address.host.c_str(),
        address.port);
    }
    else
    {
      enet_peer_reset(_server);
      _server = nullptr;
      throw std::runtime_error("Failed to connect to server");
    }
  }

  void NetworkClient::Disconnect()
  {
    _eventBus->Publish(Net::Disconnect{});
    enet_peer_disconnect_now(_server, 0);
    _server = nullptr;
  }

  bool NetworkClient::IsConnected() const
  {
    return _server != nullptr;
  }

  void NetworkClient::SendSimpleMessage(const char* message)
  {
    G_ASSERT(_server && "Must be connected to a server to send a message");
    ENetPacket* packet = enet_packet_create(message,
                                            strlen(message) + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(_server, 0, packet);
  }

  void NetworkClient::Poll([[maybe_unused]] double dt)
  {
    ENetEvent event{};
    while (enet_host_service(_client, &event, 0) > 0)
    {
      switch (event.type)
      {
      case ENET_EVENT_TYPE_RECEIVE:
        printf("A packet of length %u containing %s was received from %s on channel %u\n",
          (unsigned)event.packet->dataLength,
          (char*)event.packet->data,
          (char*)event.peer->data,
          event.channelID);

        enet_packet_destroy(event.packet);

        break;
      case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        printf("Disconnected from server (server timed out)\n");
        _server = nullptr;

        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnected from server\n");
        event.peer->data = nullptr;
        _server = nullptr;

        break;
      default:
        printf("Unknown event type\n");
        break;
      }
    }
  }
}