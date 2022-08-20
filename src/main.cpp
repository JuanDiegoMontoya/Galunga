#include "Application.h"

//#define ENET_IMPLEMENTATION
//#include "enet/enet.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include <entt/entt.hpp>

int main()
{
  auto app = Application("Galunga");
  app.Run();

  return 0;
}