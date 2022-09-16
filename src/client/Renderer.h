#pragma once
#include <string_view>
#include <vector>
#include <glm/mat3x2.hpp>
#include <glm/vec4.hpp>

struct GLFWwindow;

namespace Fwog
{
  class Texture;
}

namespace client
{
  struct RenderableSprite
  {
    glm::mat3x2 transform;
    const Fwog::Texture* texture;
    uint32_t spriteIndex;
    glm::u8vec4 tint;
  };

  class Renderer
  {
  public:
    Renderer(GLFWwindow* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void DrawBackground(const Fwog::Texture& texture);
    void DrawSprites(std::vector<RenderableSprite> sprites);

    struct Resources;

  private:
    Resources* _resources;
  };
}