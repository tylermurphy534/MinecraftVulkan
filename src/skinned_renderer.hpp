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

class SkinnedRenderer {

  public:

    SkinnedRenderer(std::vector<xe::Image*> &images);

    ~SkinnedRenderer() {};

    SkinnedRenderer(const SkinnedRenderer&) = delete;
    SkinnedRenderer operator=(const SkinnedRenderer&) = delete;

    void render(std::vector<xe::GameObject> &gameObjects, xe::Camera &xeCamera);

  private:
    std::unique_ptr<xe::RenderSystem> xeRenderSystem;

};

}