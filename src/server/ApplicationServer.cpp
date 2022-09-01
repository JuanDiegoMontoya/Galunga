#include "ApplicationServer.h"
#include "utils/Timer.h"
#include "server/net/NetworkServer.h"
#include <utility>

ApplicationServer::ApplicationServer(std::string title, EventBus* eventBus, Net::NetworkServer* networkServer)
  : _title(std::move(title)),
  _eventBus(eventBus),
  _networkServer(networkServer)
{
}

ApplicationServer::~ApplicationServer()
{
}

void ApplicationServer::Run()
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
    }
  }
}
