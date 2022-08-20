#pragma once
#include <unordered_map>
#include <string>
#include <string_view>
#include <Fwog/Texture.h>

struct TextureID
{
  const Fwog::Texture* arrayTexture;
  uint32_t layer;
};

class ArrayTextureManager
{
public:
  ArrayTextureManager(Fwog::Extent2D dimensions, uint32_t maxLayers, Fwog::Format format);
  TextureID LoadFromFile(std::string name, std::string_view path);
  [[nodiscard]] TextureID Get(std::string_view name);

protected:
  // consider a slot map if this ever becomes a bottleneck
  std::unordered_map<std::string, uint32_t> _layerIndices;
  uint32_t _currentIndex = 0;
  Fwog::Texture _texture;
};