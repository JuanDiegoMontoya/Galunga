#include "client/ApplicationClient.h"
#include "client/net/NetworkClient.h"
#include "utils/EventBus.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENET_IMPLEMENTATION
#include "enet/enet.h"

int main(int, const char* const*)
{
  EventBus eventBus;
  auto client = Net::NetworkClient(&eventBus);
  auto app = ApplicationClient("Galunga", &eventBus, &client);
  app.Run();

  return 0;
}