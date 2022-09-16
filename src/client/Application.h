#pragma once
#include <string>

class EventBus;
struct GLFWwindow;

namespace client
{
  namespace net
  {
    class Host;
  }

  namespace input
  {
    class InputManager;
  }

  class Application
  {
  public:
    Application(std::string title, EventBus* eventBus, net::Host* networkClient);
    ~Application();

    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();

  private:

    std::string _title;
    EventBus* _eventBus;
    client::net::Host* _networkClient;
    GLFWwindow* _window;
    input::InputManager* _input;
    double _simulationTick = 1.0 / 60.0;
  };
}