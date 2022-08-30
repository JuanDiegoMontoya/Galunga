#include "Renderer.h"
#include "Gassert.h"
#include "utils/LoadFile.h"
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
#include <algorithm>
#include <execution>
#include <atomic>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if !defined(NDEBUG)
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
#endif

struct SpriteUniforms
{
  glm::mat3x2 transform;
  uint32_t spriteIndex;
  glm::u8vec4 tint4x8;
};

struct FrameUniforms
{
  glm::mat4 viewProj;
};

struct Renderer::Resources
{
  // resources that need to be recreated when the window resizes
  struct Frame
  {
    uint32_t width{};
    uint32_t height{};
    Fwog::Texture output_ldr;
    float AspectRatio()
    {
      return static_cast<float>(width) / height;
    }
  };

  Frame frame;
  Fwog::GraphicsPipeline backgroundPipeline;
  Fwog::GraphicsPipeline quadBatchedPipeline;
  Fwog::TypedBuffer<SpriteUniforms> spritesUniformsBuffer;
  Fwog::TypedBuffer<FrameUniforms> frameUniformsBuffer;
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

  glEnable(GL_FRAMEBUFFER_SRGB);

  int iframebufferWidth{};
  int iframebufferHeight{};
  glfwGetFramebufferSize(window, &iframebufferWidth, &iframebufferHeight);
  uint32_t framebufferWidth = static_cast<uint32_t>(iframebufferWidth);
  uint32_t framebufferHeight = static_cast<uint32_t>(iframebufferHeight);

  _resources = new Resources(
    {
      .frame = {.width = framebufferWidth,
                .height = framebufferHeight,
                .output_ldr = Fwog::CreateTexture2D({framebufferWidth, framebufferHeight}, Fwog::Format::R8G8B8A8_SRGB) },
      .spritesUniformsBuffer = Fwog::TypedBuffer<SpriteUniforms>(1024, Fwog::BufferStorageFlag::DYNAMIC_STORAGE),
      .frameUniformsBuffer = Fwog::TypedBuffer<FrameUniforms>(Fwog::BufferStorageFlag::DYNAMIC_STORAGE)
    });

  auto bg_vs = Fwog::Shader(Fwog::PipelineStage::VERTEX_SHADER, LoadFile("assets/shaders/FullScreenTri.vert.glsl"));
  auto bg_fs = Fwog::Shader(Fwog::PipelineStage::FRAGMENT_SHADER, LoadFile("assets/shaders/Texture.frag.glsl"));
  _resources->backgroundPipeline = Fwog::CompileGraphicsPipeline({ .vertexShader = &bg_vs, .fragmentShader = &bg_fs});

  auto quadBatched_vs = Fwog::Shader(Fwog::PipelineStage::VERTEX_SHADER, LoadFile("assets/shaders/QuadBatched.vert.glsl"));
  auto quadBatched_fs = Fwog::Shader(Fwog::PipelineStage::FRAGMENT_SHADER, LoadFile("assets/shaders/QuadBatched.frag.glsl"));
  _resources->quadBatchedPipeline = Fwog::CompileGraphicsPipeline({ 
    .vertexShader = &quadBatched_vs, 
    .fragmentShader = &quadBatched_fs,
    .inputAssemblyState = {.topology = Fwog::PrimitiveTopology::TRIANGLE_FAN} });
}

Renderer::~Renderer()
{
  delete _resources;
}

void Renderer::DrawBackground(const Fwog::Texture& texture)
{
  G_ASSERT(texture.CreateInfo().imageType == Fwog::ImageType::TEX_2D);
  Fwog::BeginSwapchainRendering({ .viewport = {.drawRect = {.offset{}, .extent{1280, 720}}},
                                  .clearColorOnLoad = true,
                                  .clearColorValue = {.f = {.3f, .8f, .2f, 1.f}} });
  Fwog::Cmd::BindGraphicsPipeline(_resources->backgroundPipeline);
  Fwog::Cmd::BindSampledImage(0, texture, Fwog::Sampler(Fwog::SamplerState{}));
  Fwog::Cmd::Draw(3, 1, 0, 0);
  Fwog::EndRendering();
}

void Renderer::DrawSprites(std::vector<RenderableSprite> sprites)
{
  if (sprites.empty())
  {
    return;
  }

  auto view = glm::mat4(1);
  auto proj = glm::ortho<float>(-10 * _resources->frame.AspectRatio(), 10 * _resources->frame.AspectRatio(), -10, 10, -1, 1);
  auto cameraUniformBuffer = Fwog::Buffer(proj * view);

  // sort sprites by texture
  std::sort(std::execution::par,
    sprites.begin(),
    sprites.end(),
    [](const RenderableSprite& lhs, const RenderableSprite& rhs)
    {
      return lhs.texture < rhs.texture;
    });

  std::vector<SpriteUniforms> spritesUniforms;

  spritesUniforms.resize(sprites.size());
  std::transform(std::execution::par,
    sprites.begin(),
    sprites.end(),
    spritesUniforms.begin(),
    [](const RenderableSprite& sprite)
    {
      SpriteUniforms spriteUniforms
      {
        .transform = sprite.transform,
        .spriteIndex = sprite.spriteIndex,
        .tint4x8 = sprite.tint
      };
      return spriteUniforms;
    });

  // geometric expansion so we don't spam buffers
  if (_resources->spritesUniformsBuffer.Size() < spritesUniforms.size() * sizeof(SpriteUniforms))
  {
    _resources->spritesUniformsBuffer = Fwog::TypedBuffer<SpriteUniforms>(spritesUniforms.size() * 2, Fwog::BufferStorageFlag::DYNAMIC_STORAGE);
  }

  _resources->spritesUniformsBuffer.SubData(std::span(spritesUniforms), 0);

  Fwog::BeginSwapchainRendering({ .viewport = {.drawRect = {.offset{}, .extent{1280, 720} } } });
  Fwog::Cmd::BindGraphicsPipeline(_resources->quadBatchedPipeline);
  Fwog::Cmd::BindUniformBuffer(0, cameraUniformBuffer, 0, cameraUniformBuffer.Size());
  Fwog::Cmd::BindStorageBuffer(0, _resources->spritesUniformsBuffer, 0, _resources->spritesUniformsBuffer.Size());

  size_t firstInstance = 0;
  for (size_t i = 0; i < sprites.size(); i++)
  {
    const auto& sprite = sprites[i];

    // trigger draw if last sprite or if next sprite has a different texture
    if (i == sprites.size() - 1 || sprite.texture != sprites[i + 1].texture)
    {
      Fwog::Cmd::BindSampledImage(0, *sprite.texture, Fwog::Sampler(Fwog::SamplerState{}));
      Fwog::Cmd::Draw(4, static_cast<uint32_t>(i + 1 - firstInstance), 0, static_cast<uint32_t>(firstInstance));

      firstInstance = i + 1;
    }
  }
  Fwog::EndRendering();
}