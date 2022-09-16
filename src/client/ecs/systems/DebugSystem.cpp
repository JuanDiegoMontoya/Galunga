#include "DebugSystem.h"
#include "client/net/Host.h"
#include "net/Address.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/gl.h>

namespace client::ecs
{
  DebugSystem::DebugSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window, client::net::Host* networkClient)
    : System(scene, eventBus),
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
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("hello");
    ImGui::Text("Framerate: %fHz", 1.0 / dt);
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