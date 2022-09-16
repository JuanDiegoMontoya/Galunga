#pragma once
#include <string>

class EventBus;

namespace server
{
  namespace net
  {
    class Host;
  }

  class Application
  {
  public:
    Application(std::string title, EventBus* eventBus, net::Host* networkServer);
    ~Application();

    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();

  private:

    std::string _title;
    EventBus* _eventBus;
    net::Host* _networkServer;
    double _simulationTick = 1.0 / 60.0;
    bool shouldClose = false;
  };
}