#pragma once
#include <string>

class EventBus;

namespace Net
{
  class NetworkServer;
}

class ApplicationServer
{
public:
  ApplicationServer(std::string title, EventBus* eventBus, Net::NetworkServer* networkServer);
  ~ApplicationServer();

  ApplicationServer(const ApplicationServer&) = delete;
  ApplicationServer(ApplicationServer&&) = delete;
  ApplicationServer& operator=(const ApplicationServer&) = delete;
  ApplicationServer& operator=(ApplicationServer&&) = delete;

  void Run();

private:

  std::string _title;
  EventBus* _eventBus;
  Net::NetworkServer* _networkServer;
  double _simulationTick = 1.0 / 60.0;
  bool shouldClose = false;
};