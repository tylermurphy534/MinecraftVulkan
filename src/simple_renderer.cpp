#include "simple_renderer.hpp"

namespace app {

SimpleRenderer::SimpleRenderer(xe::XeEngine &xeEngine, xe::XeImage *xeImage) 
  : xeRenderSystem{xeEngine, "res/shaders/simple_shader.vert.spv", "res/shaders/simple_shader.frag.spv", sizeof(PushConstant), sizeof(UniformBuffer), xeImage} {};

void SimpleRenderer::render(std::vector<xe::XeGameObject> &gameObjects, xe::XeCamera &xeCamera, xe::XeImage *xeImage) {

  xeRenderSystem.loadTexture(xeImage);

  xeRenderSystem.start();

  UniformBuffer ubo{};
  ubo.projectionView = xeCamera.getProjection() * xeCamera.getView();
  xeRenderSystem.loadUniformObject(&ubo);

  for(auto &obj : gameObjects) {
    PushConstant pc{};
    pc.modelMatrix = obj.transform.mat4();
    pc.normalMatrix = obj.transform.normalMatrix();
    xeRenderSystem.loadPushConstant(&pc);
    xeRenderSystem.render(obj);
  }

  xeRenderSystem.stop();

}

}