#include "Application.h"
#include "utils/EventBus.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
  EventBus eventBus;
  auto app = Application("Galunga", &eventBus);
  app.Run();

  return 0;
}