#include "server/Application.h"
#include "server/net/Host.h"
#include "utils/EventBus.h"

#define ENET_IMPLEMENTATION
#include <enet/enet.h>

int main(int, const char* const*)
{
  EventBus eventBus;
  auto client = server::net::Host(&eventBus);
  auto app = server::Application("Galunga", &eventBus, &client);
  app.Run();

  return 0;
}