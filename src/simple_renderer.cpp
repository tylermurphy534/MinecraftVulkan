#include "simple_renderer.hpp"

namespace app {

SimpleRenderer::SimpleRenderer(xe::Engine &xeEngine, xe::Image *xeImage) {
  xeRenderSystem = xe::RenderSystem::Builder(xeEngine, "res/shaders/simple_shader.vert.spv", "res/shaders/simple_shader.frag.spv")
    .addVertexBinding(0, 3, 0)
    .addVertexBinding(1, 3, 12)
    .addVertexBinding(2, 3, 24)
    .addVertexBinding(3, 2, 36)
    .setVertexSize(sizeof(Vertex))
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