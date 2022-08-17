#pragma
#include <string_view>

struct GLFWwindow;

class Renderer
{
public:
  Renderer(GLFWwindow* window);
  ~Renderer();

  Renderer(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  void DrawBackground();

  struct Resources;

private:
  Resources* _resources;
};