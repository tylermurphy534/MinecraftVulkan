#include "simple_renderer.hpp"

namespace app {

SimpleRenderer::SimpleRenderer(xe::XeEngine &xeEngine, xe::XeImage *xeImage) {
  xeRenderSystem = xe::XeRenderSystem::Builder(xeEngine, "res/shaders/simple_shader.vert.spv", "res/shaders/simple_shader.frag.spv")
    .addPushConstant(sizeof(PushConstant))
    .addUniformBinding(0, sizeof(UniformBuffer))
    .addTextureBinding(1, xeImage)
    .setCulling(true)
    .build();
}

void SimpleRenderer::render(std::vector<xe::XeGameObject> &gameObjects, xe::XeCamera &xeCamera, xe::XeImage *xeImage) {

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