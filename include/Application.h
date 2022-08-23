#pragma once
#include <string>
#include <unordered_set>

class Renderer;
class EventBus;
struct GLFWwindow;

class Application
{
public:
  Application(std::string title, EventBus* eventBus);
  ~Application();

  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;

  void Run();

private:

  std::string _title;
  EventBus* _eventBus;
  GLFWwindow* _window;
  Renderer* _renderer;
};