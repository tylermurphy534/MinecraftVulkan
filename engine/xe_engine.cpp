#include "xe_engine.hpp"
#include "xe_descriptors.hpp"

#include <string>
namespace xe {

XeEngine::XeEngine(int width, int height, std::string name) 
  : xeWindow{width, height, name}, 
    xeDevice{xeWindow}, 
    xeRenderer{xeWindow, xeDevice},
    xeCamera{} {
    loadDescriptors();
  };

void XeEngine::loadDescriptors() {
  xeDescriptorPool = XeDescriptorPool::Builder(xeDevice)
    .setMaxSets(XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();

  xeDescriptorSetLayout = XeDescriptorSetLayout::Builder(xeDevice)
    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    .build();
}

std::unique_ptr<XeRenderSystem> XeEngine::createRenderSystem(const std::string &vert, const std::string &frag, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize) {
  return std::make_unique<XeRenderSystem>(
    xeDevice,
    xeRenderer,
    *xeDescriptorPool,
    *xeDescriptorSetLayout,
    vert,
    frag,
    pushCunstantDataSize,
    uniformBufferDataSize
  );
}

std::shared_ptr<XeModel> XeEngine::createModel(const std::string &filename) {
  return XeModel::createModelFromFile(xeDevice, filename);
}

void XeEngine::render(
  XeRenderSystem& xeRenderSystem, 
  std::vector<XeGameObject>& gameObjects,
  void* pushConstantData, 
  uint32_t pushConstantSize, 
  void* uniformBufferData, 
  uint32_t uniformBufferSize) {

  auto commandBuffer = xeRenderer.getCurrentCommandBuffer();
  xeRenderer.beginSwapChainRenderPass(commandBuffer);

  xeRenderSystem.renderGameObjects(
    xeRenderer.getFrameIndex(),
    commandBuffer,
    gameObjects,
    pushConstantData,
    pushConstantSize,
    uniformBufferData,
    uniformBufferSize
  );
  
  xeRenderer.endSwapChainRenderPass(commandBuffer);
}

}