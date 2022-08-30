#include "ApplicationClient.h"
#include "Renderer.h"
#include "utils/EventBus.h"
#include "utils/Timer.h"
#include "net/client/NetworkClient.h"
#include "net/Address.h"
#include <Fwog/Texture.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

ApplicationClient::ApplicationClient(std::string title, EventBus* eventBus, Net::NetworkClient* networkClient)
  : _title(std::move(title)),
    _eventBus(eventBus),
    _networkClient(networkClient)
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

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(_window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
}

ApplicationClient::~ApplicationClient()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  delete _renderer;
  glfwTerminate();
}

void ApplicationClient::Run()
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

  Timer timer;
  double simulationAccum = 0;
  while (!glfwWindowShouldClose(_window))
  {
    double dt = timer.Elapsed_s();
    timer.Reset();

    tempAccum += dt;
    while (tempAccum > 1.0)
    {
      //std::cout << "FPS: " << 1.0 / dt << '\n';
      tempAccum -= 1.0;
    }

    for (auto& sprite : sprites)
    {
      sprite.transform[2][0] += .5f * (float)dt;
    }

    simulationAccum += dt;
    while (simulationAccum > _simulationTick)
    {
      glfwPollEvents();
      _networkClient->Poll(_simulationTick);
      simulationAccum -= _simulationTick;
    }

    if (glfwGetKey(_window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(_window, true);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("hello");
    static char buffer[512]{ "localhost" };
    static int port = 1234;
    ImGui::InputText("Server host", buffer, 512);
    ImGui::InputInt("Server port", &port, 0, 0);

    bool connected = _networkClient->IsConnected();
    if (connected) ImGui::BeginDisabled();
    if (ImGui::Button("Connect"))
    {
      try
      {
        _networkClient->ConnectToServer({ buffer, (uint16_t)port });
      }
      catch (std::exception& e)
      {
        printf("%s\n", e.what());
      }
    }
    if (connected) ImGui::EndDisabled();

    ImGui::SameLine();

    if (!connected) ImGui::BeginDisabled();
    if (ImGui::Button("Disconnect"))
    {
      _networkClient->Disconnect();
    }
    if (!connected) ImGui::EndDisabled();

    static char message[512]{ "hello" };
    ImGui::InputText("Message", message, 512);
    if (ImGui::Button("Send"))
    {
      _networkClient->SendSimpleMessage(message);
    }

    ImGui::End();

    _renderer->DrawBackground(bgTex);
    _renderer->DrawSprites(sprites);

    glDisable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    glEnable(GL_FRAMEBUFFER_SRGB);

    glfwSwapBuffers(_window);
  }
}