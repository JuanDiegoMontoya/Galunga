#pragma once
#include <ecs/systems/System.h>
#include <client/TextureManager.h>
#include <ecs/events/AddSprite.h>
#include <memory>

class Renderer;
struct GLFWwindow;

namespace client::ecs
{
  class RenderingSystem : public ::ecs::System
  {
  public:
    RenderingSystem(::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window);

    void Update(double dt) override;

  private:
    std::unique_ptr<Renderer> _renderer;
    ArrayTextureManager _textureManager;
    GLFWwindow* _window;
    std::unique_ptr<Fwog::Texture> _backgroundTexture;

    void Listener(::ecs::AddSprite& e);
  };
}