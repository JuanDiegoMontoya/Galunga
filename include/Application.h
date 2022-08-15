#pragma once
#include <string>

class Renderer;
struct GLFWwindow;

class Application
{
public:
  Application(std::string title);
  ~Application();

  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(const Application&) = delete;
  Application& operator=(Application&&) = delete;

  void Run();

private:

  GLFWwindow* _window;
  Renderer* _renderer;
  std::string _title;
};