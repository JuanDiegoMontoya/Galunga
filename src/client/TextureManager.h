#pragma once
#include <Fwog/Texture.h>
#include <string_view>

struct TextureID
{
  const Fwog::Texture* arrayTexture;
  uint32_t layer;
};

class ArrayTextureManager
{
public:
  ArrayTextureManager(Fwog::Extent2D dimensions, uint32_t maxLayers, Fwog::Format format);
  TextureID LoadFromFile(std::string_view path);

  Fwog::Texture* GetTexture() { return &_texture; }

protected:
  uint32_t _currentIndex = 0;
  Fwog::Texture _texture;
};

// maybe use this in the future for maximum flexibility

//class ArrayTextureManager
//{
//public:
//  // returns an index into the vector of texture IDs
//  uint32_t LoadFromFile(std::string_view path);
//
//private:
//
//  struct Dimensions { uint32_t x, y; };
//
//  // the order of this must be consistent with the indices passed by the server
//  std::vector<TextureID> _textureIDs;
//  std::unordered_map<Dimensions, Fwog::Texture> _textures;
//};