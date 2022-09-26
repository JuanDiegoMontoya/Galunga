#include "DebugSystem.h"
#include "client/net/Host.h"
#include "shared/net/Address.h"
#include "client/ecs/components/DebugDraw.h"
#include "client/Renderer.h"
#include "Platform.h"
#include "utils/Timer.h"
#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/gl.h>

namespace client::ecs
{
  DebugSystem::DebugSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window, net::Host* networkClient, Renderer* renderer)
    : System(scene, eventBus),
      _renderer(renderer),
      _window(window),
      _networkClient(networkClient)
  {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(_window, false);
    ImGui_ImplOpenGL3_Init("#version 450 core");
    ImGui::StyleColorsDark();
  }

  DebugSystem::~DebugSystem()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void DebugSystem::Update(double dt)
  {
    // draw debug primitives
    auto viewLine = SceneRegistry()->view<ecs::DebugLine>();
    auto viewBox = SceneRegistry()->view<ecs::DebugBox>();
    auto viewCircle = SceneRegistry()->view<ecs::DebugCircle>();
    std::vector<ecs::DebugLine> lines;
    std::vector<ecs::DebugBox> boxes;
    std::vector<ecs::DebugCircle> circles;
    lines.reserve(viewLine.size());
    boxes.reserve(viewBox.size());
    circles.reserve(viewCircle.size());
    
    for (auto&& [_, line] : viewLine.each())
    {
      lines.push_back(line);
    }
    for (auto&& [_, box] : viewBox.each())
    {
      boxes.push_back(box);
    }
    for (auto&& [_, circle] : viewCircle.each())
    {
      circles.push_back(circle);
    }

    _renderer->DrawLines(lines);
    _renderer->DrawBoxes(boxes);
    _renderer->DrawCircles(circles);

    // draw debug GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("hello");
    ImGui::Text("Framerate: %.0fHz", 1.0 / dt);
    ImGui::Text("Entities: %d", SceneRegistry()->alive());
    if (ImGui::Button("Nuke Entities"))
    {
      SceneRegistry()->clear();
    }
    ImGui::SliderInt("Fake Lag (ms)", &_fakeLag_ms, 0, 100);
    if (_fakeLag_ms > 0)
    {
      Timer timer;
      while (int(timer.Elapsed_ms()) < _fakeLag_ms)
      {
        G_PAUSE();
      }
    }

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

    glDisable(GL_FRAMEBUFFER_SRGB);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
  }
}