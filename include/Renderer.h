#pragma
#include <string_view>

class Renderer
{
public:
  Renderer();
  ~Renderer();

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  void DrawBackground();

private:

};