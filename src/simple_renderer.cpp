#include "simple_renderer.hpp"
#include "chunk.hpp"

namespace app {

SimpleRenderer::SimpleRenderer(xe::Engine &xeEngine, std::vector<xe::Image*> &images) {
  xeRenderSystem = xe::RenderSystem::Builder(xeEngine, "res/shaders/simple_shader.vert.spv", "res/shaders/simple_shader.frag.spv")
    .addVertexBindingf(0, 3, 0) // position
    .addVertexBindingf(1, 3, 12) // normal
    .addVertexBindingf(2, 2, 24) // uvs
    .addVertexBindingi(3, 1, 32) // texture
    .setVertexSize(36)
    .addPushConstant(sizeof(PushConstant))
    .addUniformBinding(0, sizeof(UniformBuffer))
    .addTextureArrayBinding(1, images)
    .setCulling(false)
    .build();
}

void SimpleRenderer::render(std::vector<xe::GameObject> &gameObjects, xe::Camera &xeCamera) {

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