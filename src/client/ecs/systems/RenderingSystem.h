#pragma once
#include <ecs/systems/System.h>
#include <client/TextureManager.h>
#include <ecs/events/AddSprite.h>
#include <memory>

namespace client
{
  class Renderer;
}

struct GLFWwindow;

namespace client::ecs
{
  class RenderingSystem : public shared::ecs::System
  {
  public:
    RenderingSystem(shared::ecs::Scene* scene, EventBus* eventBus, GLFWwindow* window, Renderer* renderer);

    void Update(double dt) override;

  private:
    Renderer* _renderer;
    ArrayTextureManager _textureManager;
    GLFWwindow* _window;
    std::unique_ptr<Fwog::Texture> _backgroundTexture;

    void Listener(shared::ecs::AddSprite& e);
  };
}