#include "Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
  auto app = Application("Galunga");
  app.Run();

  return 0;
}