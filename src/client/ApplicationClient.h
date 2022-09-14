#pragma once
#include <string>

class EventBus;
struct GLFWwindow;

namespace Net
{
  class NetworkClient;
}

namespace Input
{
  class InputManager;
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
  Input::InputManager* _input;
  double _simulationTick = 1.0 / 60.0;
};