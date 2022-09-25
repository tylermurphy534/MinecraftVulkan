#pragma once

#include "xe_render_system.hpp"
#include <string>

namespace app {

struct UniformBuffer {
  alignas(16) glm::mat4 projectionView{1.f};
  alignas(4) glm::vec3 lightDirection = glm::normalize(glm::vec3{-1.f, 3.f, 1.f});
};

struct PushConstant {
  alignas(16) glm::mat4 modelMatrix{1.f};
  alignas(16) glm::mat4 normalMatrix{1.f};
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec2 uv;
};

class SimpleRenderer {

  public:

    SimpleRenderer(xe::Engine &xeEngine, xe::Image *xeImage);

    ~SimpleRenderer() {};

    SimpleRenderer(const SimpleRenderer&) = delete;
    SimpleRenderer operator=(const SimpleRenderer&) = delete;

    void render(std::vector<xe::GameObject> &gameObjects, xe::Camera &xeCamera, xe::Image *xeImage);

  private:
    std::unique_ptr<xe::RenderSystem> xeRenderSystem;

};

}