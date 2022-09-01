#pragma once
#include <string>

class Renderer;
class EventBus;
struct GLFWwindow;

namespace Net
{
  class NetworkClient;
}

class ApplicationClient
{
public:
  ApplicationClient(std::string title, EventBus* eventBus, Net::NetworkClient* networkClient);
  ~ApplicationClient();

  ApplicationClient(const ApplicationClient&) = delete;
  ApplicationClient(ApplicationClient&&) = delete;
  ApplicationClient& operator=(const ApplicationClient&) = delete;
  ApplicationClient& operator=(ApplicationClient&&) = delete;

  void Run();

private:

  std::string _title;
  EventBus* _eventBus;
  Net::NetworkClient* _networkClient;
  GLFWwindow* _window;
  Renderer* _renderer;
  double _simulationTick = 1.0 / 60.0;
};