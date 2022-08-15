#include "Renderer.h"
#include <Fwog/Rendering.h>
#include <Fwog/Pipeline.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

static void GLAPIENTRY glErrorCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  const GLchar* message,
  [[maybe_unused]] const void* userParam)
{
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 0)
    return;

  std::stringstream errStream;
  errStream << "OpenGL Debug message (" << id << "): " << message << '\n';

  switch (source)
  {
  case GL_DEBUG_SOURCE_API:             errStream << "Source: API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   errStream << "Source: Window Manager"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: errStream << "Source: Shader Compiler"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:     errStream << "Source: Third Party"; break;
  case GL_DEBUG_SOURCE_APPLICATION:     errStream << "Source: Application"; break;
  case GL_DEBUG_SOURCE_OTHER:           errStream << "Source: Other"; break;
  }

  errStream << '\n';

  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:               errStream << "Type: Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errStream << "Type: Deprecated Behaviour"; break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  errStream << "Type: Undefined Behaviour"; break;
  case GL_DEBUG_TYPE_PORTABILITY:         errStream << "Type: Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE:         errStream << "Type: Performance"; break;
  case GL_DEBUG_TYPE_MARKER:              errStream << "Type: Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP:          errStream << "Type: Push Group"; break;
  case GL_DEBUG_TYPE_POP_GROUP:           errStream << "Type: Pop Group"; break;
  case GL_DEBUG_TYPE_OTHER:               errStream << "Type: Other"; break;
  }

  errStream << '\n';

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    errStream << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    errStream << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    errStream << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    errStream << "Severity: notification";
    break;
  }

  std::cout << errStream.str() << '\n';
}

Renderer::Renderer()
{
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    throw std::runtime_error("Failed to initialize OpenGL");
  }

#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glErrorCallback, NULL);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
}

Renderer::~Renderer()
{
}

void Renderer::DrawBackground()
{
  Fwog::BeginSwapchainRendering({ .viewport = {.drawRect = {.offset{}, .extent{1280, 720}}},
                                  .clearColorOnLoad = true,
                                  .clearColorValue = {.f = {.3f, .8f, .2f, 1.f}} });

  Fwog::EndRendering();
}
