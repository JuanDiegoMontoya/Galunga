#include "Application.h"
#include "net/client/GameClient.h"
#include "utils/EventBus.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
  EventBus eventBus;
  auto app = Application("Galunga", &eventBus);
  //auto client = Net::GameClient(&eventBus);
  app.Run();

  return 0;
}