#include "Application.h"
#include "utils/Timer.h"
#include "server/net/Host.h"
#include <utility>

namespace server
{
  Application::Application(std::string title, EventBus* eventBus, net::Host* networkServer)
    : _title(std::move(title)),
    _eventBus(eventBus),
    _networkServer(networkServer)
  {
  }

  Application::~Application()
  {
  }

  void Application::Run()
  {
    Timer timer;
    double simulationAccum = 0;
    while (!shouldClose)
    {
      double dt = timer.Elapsed_s();
      timer.Reset();

      simulationAccum += dt;
      while (simulationAccum > _simulationTick)
      {
        _networkServer->Poll(_simulationTick);
        simulationAccum -= _simulationTick;
      }
    }
  }
}