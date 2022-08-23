#include "Application.h"
#include "Renderer.h"
#include "utils/EventBus.h"
#include <Fwog/Texture.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <stdexcept>
#include <string>

// temporary includes
#include <iostream>
#include <stb_image.h>
#include <vector>
#include "TextureManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

Application::Application(std::string title, EventBus* eventBus)
  : _title(std::move(title)),
    _eventBus(eventBus)
{
  // init everything
  if (!glfwInit())
  {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwSetErrorCallback([](int, const char* desc)
    {
      throw std::runtime_error(std::string("GLFW error: ") + desc + '\n');
    });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, false); // TODO: load from config
  glfwWindowHint(GLFW_DECORATED, true); // TODO: load from config
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

  // TODO: load parameters from config
  _window = glfwCreateWindow(1280, 
                             720, 
                             _title.c_str(),
                             nullptr, 
                             nullptr);

  if (!_window)
  {
    throw std::runtime_error("Failed to create _window");
  }

  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  _renderer = new Renderer(_window);
}

Application::~Application()
{
  delete _renderer;
  glfwTerminate();
}

void Application::Run()
{
  int x{};
  int y{};
  int nc{};
  stbi_set_flip_vertically_on_load(true);
  auto* pixels = stbi_load("assets/textures/test.png", &x, &y, &nc, 4);

  auto bgTex = Fwog::CreateTexture2D({ static_cast<uint32_t>(x), static_cast<uint32_t>(y) }, Fwog::Format::R8G8B8A8_SRGB);
  bgTex.SubImage({ .dimension = Fwog::UploadDimension::TWO,
                   .size = bgTex.Extent(),
                   .format = Fwog::UploadFormat::RGBA,
                   .type = Fwog::UploadType::UBYTE,
                   .pixels = pixels });

  stbi_image_free(pixels);

  auto textureManager32 = ArrayTextureManager({ 32, 32 }, 100, Fwog::Format::R8G8B8A8_SRGB);
  auto trollge = textureManager32.LoadFromFile("trollge", "assets/textures/test.png");
  auto orang = textureManager32.LoadFromFile("orang", "assets/textures/test2.png");

  auto textureManager64 = ArrayTextureManager({ 64, 64 }, 100, Fwog::Format::R8G8B8A8_SRGB);
  auto froge = textureManager64.LoadFromFile("orang", "assets/textures/frog_pink.png");

  std::vector<RenderableSprite> sprites;

  for (int xx = -50; xx < 50; xx++)
  for (int yy = -50; yy < 50; yy++)
  {
    RenderableSprite rs{};
    rs.transform = glm::mat3x2(1);
    rs.transform[0][0] = .25f;
    rs.transform[1][1] = .25f;
    rs.transform[2] = glm::vec2(xx, yy) / 2.f;
    if (yy % 3 == 0)
    {
      rs.texture = trollge.arrayTexture;
      rs.spriteIndex = trollge.layer;
    }
    else if (yy % 3 == 1)
    {
      rs.texture = orang.arrayTexture;
      rs.spriteIndex = orang.layer;
    }
    else
    {
      rs.texture = froge.arrayTexture;
      rs.spriteIndex = froge.layer;
    }
    rs.tint = glm::u8vec4(255, 255, 255, 255);
    sprites.push_back(rs);
  }

  double tempAccum = 0;
  double prevTime = glfwGetTime();
  while (!glfwWindowShouldClose(_window))
  {
    double curTime = glfwGetTime();
    double dt = curTime - prevTime;
    prevTime = curTime;

    tempAccum += dt;
    while (tempAccum > 1.0)
    {
      std::cout << "FPS: " << 1.0 / dt << '\n';
      tempAccum -= 1.0;
    }

    for (auto& sprite : sprites)
    {
      sprite.transform[2][0] += .5f * (float)dt;
    }

    glfwPollEvents();

    if (glfwGetKey(_window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(_window, true);
    }

    _renderer->DrawBackground(bgTex);
    _renderer->DrawSprites(sprites);

    glfwSwapBuffers(_window);
  }
}