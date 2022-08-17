#include "Renderer.h"
#include <Fwog/Rendering.h>
#include <Fwog/Pipeline.h>
#include <Fwog/Texture.h>
#include <Fwog/Buffer.h>
#include <Fwog/Shader.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image.h>

std::string LoadFile(std::string_view path)
{
  std::ifstream file{ path.data() };
  return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

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

struct Renderer::Resources
{
  // resources that need to be recreated when the window resizes
  struct Frame
  {
    uint32_t width{};
    uint32_t height{};
    Fwog::Texture output_ldr;
  };

  Frame frame;
  Fwog::Texture testTex;
  Fwog::GraphicsPipeline testPipe;
};

Renderer::Renderer(GLFWwindow* window)
{
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0)
  {
    throw std::runtime_error("Failed to initialize OpenGL");
  }

#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glErrorCallback, nullptr);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

  int iframebufferWidth{};
  int iframebufferHeight{};
  glfwGetFramebufferSize(window, &iframebufferWidth, &iframebufferHeight);
  uint32_t framebufferWidth = static_cast<uint32_t>(iframebufferWidth);
  uint32_t framebufferHeight = static_cast<uint32_t>(iframebufferHeight);

  int x{};
  int y{};
  int nc{};
  stbi_set_flip_vertically_on_load(true);
  auto* data = stbi_load("assets/textures/test.png", &x, &y, &nc, 4);

  _resources = new Resources(
    {
      .frame = { .width = framebufferWidth,
                 .height = framebufferHeight,
                 .output_ldr = Fwog::CreateTexture2D({framebufferWidth, framebufferHeight}, Fwog::Format::R8G8B8A8_SRGB) },
      .testTex = Fwog::CreateTexture2D({ uint32_t(x), uint32_t(y) }, Fwog::Format::R8G8B8A8_UNORM)
    });

  _resources->testTex.SubImage({ .dimension = Fwog::UploadDimension::TWO,
                                 .size = _resources->testTex.Extent(),
                                 .format = Fwog::UploadFormat::RGBA,
                                 .type = Fwog::UploadType::UBYTE,
                                 .pixels = data });

  stbi_image_free(data);

  auto vs = Fwog::Shader(Fwog::PipelineStage::VERTEX_SHADER, LoadFile("assets/shaders/FullScreenTri.vert.glsl"));
  auto fs = Fwog::Shader(Fwog::PipelineStage::FRAGMENT_SHADER, LoadFile("assets/shaders/Texture.frag.glsl"));
  _resources->testPipe = Fwog::CompileGraphicsPipeline({ .vertexShader = &vs,
                                                         .fragmentShader = &fs});
}

Renderer::~Renderer()
{
  delete _resources;
}

void Renderer::DrawBackground()
{
  Fwog::BeginSwapchainRendering({ .viewport = {.drawRect = {.offset{}, .extent{1280, 720}}},
                                  .clearColorOnLoad = true,
                                  .clearColorValue = {.f = {.3f, .8f, .2f, 1.f}} });
  Fwog::Cmd::BindGraphicsPipeline(_resources->testPipe);
  Fwog::Cmd::BindSampledImage(0, _resources->testTex, Fwog::Sampler(Fwog::SamplerState{}));
  Fwog::Cmd::Draw(3, 1, 0, 0);
  Fwog::EndRendering();
}
