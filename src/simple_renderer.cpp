#include "simple_renderer.hpp"
#include "xe_render_system.hpp"

#include <iostream>

namespace app {

SimpleRenderer::SimpleRenderer(xe::Engine &xeEngine, xe::Image *xeImage) {
  xeRenderSystem = xe::RenderSystem::Builder(xeEngine, "res/shaders/simple_shader.vert.spv", "res/shaders/simple_shader.frag.spv")
    .addPushConstant(sizeof(PushConstant))
    .addUniformBinding(0, sizeof(UniformBuffer))
    .addTextureBinding(1, xeImage)
    .setCulling(true)
    .build();
}

void SimpleRenderer::render(std::vector<xe::GameObject> &gameObjects, xe::Camera &xeCamera, xe::Image *xeImage) {

  xeRenderSystem->loadTexture(1, xeImage);

  xeRenderSystem->start();

  UniformBuffer ubo{};
  ubo.projectionView = xeCamera.getProjection() * xeCamera.getView();
  xeRenderSystem->loadUniformObject(0, &ubo);

  for(auto &obj : gameObjects) {
    PushConstant pc{};
    pc.modelMatrix = obj.transform.mat4();
    pc.normalMatrix = obj.transform.normalMatrix();
    xeRenderSystem->loadPushConstant(&pc);
    xeRenderSystem->render(obj);
  }

  xeRenderSystem->stop();

}

}