#pragma once
#include <ecs/systems/System.h>

struct GLFWwindow;

namespace client
{
  class Renderer;
}

namespace client::net
{
  class Host;
}

namespace client::ecs
{
  class DebugSystem : public shared::ecs::System
  {
  public:
    DebugSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window, net::Host* networkClient, Renderer* renderer);
    ~DebugSystem();

    void Update(double dt) override;

  private:
    Renderer* _renderer;
    GLFWwindow* _window;
    net::Host* _networkClient;
  };
}