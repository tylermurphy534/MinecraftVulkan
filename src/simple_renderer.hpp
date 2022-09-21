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

class SimpleRenderer {

  public:

    SimpleRenderer(xe::XeEngine &xeEngine, xe::XeImage *xeImage);

    ~SimpleRenderer() {};

    SimpleRenderer(const SimpleRenderer&) = delete;
    SimpleRenderer operator=(const SimpleRenderer&) = delete;

    void render(std::vector<xe::XeGameObject> &gameObjects, xe::XeCamera &xeCamera, xe::XeImage *xeImage);

  private:
    std::unique_ptr<xe::XeRenderSystem> xeRenderSystem;

};

}