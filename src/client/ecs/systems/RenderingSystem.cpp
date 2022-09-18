#include "RenderingSystem.h"
#include <ecs/components/core/Sprite.h>
#include <ecs/components/core/Transform.h>
#include <client/Renderer.h>
#include <utils/EventBus.h>
#include <entt/entity/registry.hpp>
#include <stb_image.h>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glad/gl.h>
#include <vector>
#include <utility>
#include <algorithm>

namespace client::ecs
{
  RenderingSystem::RenderingSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window)
    : System(scene, eventBus),
      _renderer(std::make_unique<Renderer>(window)),
      _textureManager({ 32, 32 }, 100, Fwog::Format::R8G8B8A8_SRGB),
      _window(window)
  {
    int x{};
    int y{};
    int nc{};
    stbi_set_flip_vertically_on_load(true);
    auto* pixels = stbi_load("assets/textures/test.png", &x, &y, &nc, 4);

    _backgroundTexture = std::make_unique<Fwog::Texture>(Fwog::CreateTexture2D({ static_cast<uint32_t>(x), static_cast<uint32_t>(y) }, Fwog::Format::R8G8B8A8_SRGB));
    _backgroundTexture->SubImage({ .dimension = Fwog::UploadDimension::TWO,
                     .size = _backgroundTexture->Extent(),
                     .format = Fwog::UploadFormat::RGBA,
                     .type = Fwog::UploadType::UBYTE,
                     .pixels = pixels });

    stbi_image_free(pixels);

    _eventBus->Subscribe(this, &RenderingSystem::Listener);
  }

  void RenderingSystem::Update([[maybe_unused]] double dt)
  {
    glEnable(GL_FRAMEBUFFER_SRGB);

    _renderer->DrawBackground(*_backgroundTexture);

    std::vector<RenderableSprite> sprites;

    auto view = SceneRegistry()->view<shared::ecs::Transform, shared::ecs::Sprite>();
    const auto* tex = _textureManager.GetTexture();

    sprites.reserve(view.size_hint());

    std::for_each(view.begin(), view.end(),
      [&sprites, tex, view](auto entity)
      {
        const auto&& [transform, sprite] = view.get<shared::ecs::Transform, shared::ecs::Sprite>(entity);
        auto model = glm::mat3x2(1);
        //model = glm::scale(glm::translate(glm::mat3(1)) * glm::rotate(glm::mat3(1), transform.rotation))
        model = glm::scale(glm::rotate(glm::translate(glm::mat3(1), transform.translation), transform.rotation), transform.scale);

        sprites.push_back(
          RenderableSprite{
          .transform = model,
          .texture = tex,
          .spriteIndex = sprite.index,
          .tint = glm::u8vec4(255)
        });
      });

    _renderer->DrawSprites(std::move(sprites));
  }

  void RenderingSystem::Listener(shared::ecs::AddSprite& e)
  {
    _textureManager.LoadFromFile(e.path);
  }
}