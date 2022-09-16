#include "TextureManager.h"
#include "Exception.h"
#include "utils/LoadFile.h"
#include <stb_image.h>

namespace client
{
  ArrayTextureManager::ArrayTextureManager(Fwog::Extent2D dimensions, uint32_t maxLayers, Fwog::Format format)
    : _texture(Fwog::TextureCreateInfo{
        .imageType = Fwog::ImageType::TEX_2D_ARRAY,
        .format = format,
        .extent = { dimensions.width, dimensions.height, 0 },
        .mipLevels = 1,
        .arrayLayers = maxLayers })
  {
  }

  TextureID ArrayTextureManager::LoadFromFile(std::string_view path)
  {
    if (_currentIndex + 1 >= _texture.CreateInfo().arrayLayers)
    {
      throw Exception("Failed to load image. Capacity reached.");
    }

    stbi_set_flip_vertically_on_load(true);

    int x{};
    int y{};
    int nc{};
    auto* pixels = stbi_load(path.data(), &x, &y, &nc, 4);

    if (pixels == nullptr)
    {
      throw Exception(std::string("Failed to load image: ") + path.data());
    }

    if (static_cast<uint32_t>(x) != _texture.Extent().width || static_cast<uint32_t>(y) != _texture.Extent().height)
    {
      throw Exception(std::string("Image dimensions do not match array texture's. Path: ") + path.data());
    }

    _texture.SubImage({ .dimension = Fwog::UploadDimension::THREE,
                        .offset = { 0, 0, _currentIndex },
                        .size = { static_cast<uint32_t>(x), static_cast<uint32_t>(y), 1 },
                        .format = Fwog::UploadFormat::RGBA,
                        .type = Fwog::UploadType::UBYTE,
                        .pixels = pixels });

    stbi_image_free(pixels);

    return { &_texture, _currentIndex++ };
  }
}