#include "Host.h"
#include <enet/enet.h>
#include <cstdio>
#include <stdexcept>

namespace server::net
{
  Host::Host(EventBus* eventBus)
    : _eventBus(eventBus)
  {
    if (enet_initialize() != 0)
    {
      throw std::runtime_error("An error occurred while initializing ENet");
    }

    ENetAddress address{};
    address.host = ENET_HOST_ANY; // localhost
    address.port = 1234;
    address.sin6_scope_id = 0;

    _server = enet_host_create(&address, // server address
                               32, // max. clients/connections
                               2, // max. channels
                               0, // max. incoming bandwidth
                               0);// max. outgoing bandwidth
    
    if (_server == nullptr)
    {
      throw std::runtime_error("An error occurred while creating the server");
    }
  }

  Host::~Host()
  {
    enet_host_destroy(_server); // ENet allows null hosts here
    enet_deinitialize();
  }

  void Host::Poll([[maybe_unused]] double dt)
  {
    ENetEvent event{};
    while (enet_host_service(_server, &event, 0) > 0)
    {
      switch (event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
      {
        unsigned uhost{};
        memcpy(&uhost, &event.peer->address.host, sizeof(uhost));
        printf("A new client connected from %x:%u\n",
          uhost,
          event.peer->address.port);
        event.peer->data = const_cast<char*>("banana peer");
        _clients.insert(event.peer);

        break;
      }
      case ENET_EVENT_TYPE_RECEIVE:
        printf("A packet of length %u containing %s was received from %s on channel %u\n",
          (unsigned)event.packet->dataLength,
          (char*)event.packet->data,
          (char*)event.peer->data,
          event.channelID);

        enet_packet_destroy(event.packet);

        break;
      case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        printf("%s disconnected (timed out)\n", (char*)event.peer->data);
        _clients.erase(event.peer);

        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("%s disconnected\n", (char*)event.peer->data);
        event.peer->data = nullptr;
        _clients.erase(event.peer);

        break;
      default:
        printf("Unknown event type\n");
        break;
      }
    }
  }
}