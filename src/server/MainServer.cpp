#include "server/ApplicationServer.h"
#include "server/net/NetworkServer.h"
#include "utils/EventBus.h"

#define ENET_IMPLEMENTATION
#include <enet/enet.h>

int main(int, const char* const*)
{
  EventBus eventBus;
  auto client = Net::NetworkServer(&eventBus);
  auto app = ApplicationServer("Galunga", &eventBus, &client);
  app.Run();

  return 0;
}