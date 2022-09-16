#include "client/Application.h"
#include "client/net/Host.h"
#include "utils/EventBus.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENET_IMPLEMENTATION
#include "enet/enet.h"

int main(int, const char* const*)
{
  EventBus eventBus;
  auto client = client::net::Host(&eventBus);
  auto app = client::Application("Galunga", &eventBus, &client);
  app.Run();

  return 0;
}