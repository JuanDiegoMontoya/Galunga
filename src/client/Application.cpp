#include "Application.h"
#include "Renderer.h"
#include "Input.h"
#include "utils/EventBus.h"
#include "utils/Timer.h"
#include "client/net/Host.h"
#include <ecs/Scene.h>
#include <Fwog/Texture.h>
#include <GLFW/glfw3.h>
#include <ecs/systems/core/LifetimeSystem.h>
#include <client/ecs/systems/RenderingSystem.h>
#include <client/ecs/systems/DebugSystem.h>

#include <utility>
#include <stdexcept>
#include <string>

// temporary includes
#include <ecs/Entity.h>
#include <ecs/components/core/Sprite.h>
#include <ecs/components/core/Transform.h>

namespace client
{
  Application::Application(std::string title, EventBus* eventBus, net::Host* networkClient)
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

    _input = new input::InputManager(_window, _eventBus);
  }

  Application::~Application()
  {
    delete _input;
    glfwTerminate();
  }

  void Application::Run()
  {
    auto scene = shared::ecs::Scene(_eventBus);
    auto lifetimeSystem = shared::ecs::LifetimeSystem(&scene, _eventBus);
    auto renderingSystem = client::ecs::RenderingSystem(&scene, _eventBus, _window);
    auto debugSystem = client::ecs::DebugSystem(&scene, _eventBus, _window, _networkClient);

    shared::ecs::Entity entity = scene.CreateEntity("hello");
    auto& transform = entity.AddComponent<shared::ecs::Transform>();
    transform.scale = { 10, 10 };
    transform.translation = { 0, 0 };
    transform.rotation = 0;
    entity.AddComponent<shared::ecs::Sprite>().index = 0;

    _eventBus->Publish(shared::ecs::AddSprite{ .path = "assets/textures/test2.png" });

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

      transform.rotation += float(3.14 * dt);
      transform.scale *= .999f;
      transform.translation.x += float(1.0 * dt);

      renderingSystem.Update(dt);
      debugSystem.Update(dt);

      glfwSwapBuffers(_window);
    }
  }
}