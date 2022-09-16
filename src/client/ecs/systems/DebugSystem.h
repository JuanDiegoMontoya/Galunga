#pragma once
#include <ecs/systems/System.h>

struct GLFWwindow;

namespace client::net
{
  class Host;
}

namespace client::ecs
{
  class DebugSystem : public shared::ecs::System
  {
  public:
    DebugSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window, net::Host* networkClient);
    ~DebugSystem();

    void Update(double dt) override;

  private:
    GLFWwindow* _window;
    net::Host* _networkClient;
  };
}