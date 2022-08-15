#include "Application.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <utility>
#include <stdexcept>
#include <string>

Application::Application(std::string title)
  : _title(std::move(title))
{
  // init everything
  if (!glfwInit())
  {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwSetErrorCallback([](int, const char* desc)
    {
      throw std::runtime_error(std::string("GLFW error: ") + desc + '\n');
    });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, false); // TODO: load from config
  glfwWindowHint(GLFW_DECORATED, true); // TODO: load from config
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

  // TODO: load parameters from config
  _window = glfwCreateWindow(1280, 
                             720, 
                             _title.c_str(),
                             nullptr, 
                             nullptr);

  if (!_window)
  {
    throw std::runtime_error("Failed to create _window");
  }

  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  _renderer = new Renderer;
}

Application::~Application()
{
  delete _renderer;
}

void Application::Run()
{
  while (!glfwWindowShouldClose(_window))
  {
    glfwPollEvents();

    if (glfwGetKey(_window, GLFW_KEY_ESCAPE))
    {
      glfwSetWindowShouldClose(_window, true);
    }

    _renderer->DrawBackground();

    glfwSwapBuffers(_window);
  }
}
