#include "ApplicationClient.h"
#include "Renderer.h"
#include "Input.h"
#include "utils/EventBus.h"
#include "utils/Timer.h"
#include "client/net/NetworkClient.h"
#include "net/Address.h"
#include <ecs/systems/core/LifetimeSystem.h>
#include <client/ecs/systems/RenderingSystem.h>
#include <ecs/Scene.h>
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
#include <ecs/Entity.h>
#include <ecs/components/core/Sprite.h>
#include <ecs/components/core/Transform.h>

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

  _input = new Input::InputManager(_window, _eventBus);

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(_window, false);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsDark();
}

ApplicationClient::~ApplicationClient()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  delete _input;
  glfwTerminate();
}

void ApplicationClient::Run()
{
  auto scene = ecs::Scene(_eventBus);
  auto lifetimeSystem = ecs::LifetimeSystem(&scene, _eventBus);
  auto renderingSystem = client::ecs::RenderingSystem(&scene, _eventBus, _window);

  ecs::Entity entity = scene.CreateEntity("hello");
  auto& transform = entity.AddComponent<ecs::Transform>();
  transform.scale = { 10, 10 };
  transform.translation = { 0, 0 };
  transform.rotation = 0;
  entity.AddComponent<ecs::Sprite>().index = 0;

  _eventBus->Publish(ecs::AddSprite{ .path = "assets/textures/test2.png" });

  Timer timer;
  double simulationAccum = 0;
  while (!glfwWindowShouldClose(_window))
  {
    double dt = timer.Elapsed_s();
    timer.Reset();

    simulationAccum += dt;
    while (simulationAccum > _simulationTick)
    {
      _input->PollEvents(_simulationTick);
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
    ImGui::Text("Framerate: %fhz", 1.0 / dt);
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

    transform.rotation += float(3.14 * dt);
    transform.scale *= .999f;
    transform.translation.x += float(1.0 * dt);

    renderingSystem.Update(dt);

    glDisable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    glEnable(GL_FRAMEBUFFER_SRGB);

    glfwSwapBuffers(_window);
  }
}