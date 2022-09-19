#include "xe_engine.hpp"
#include "xe_descriptors.hpp"

namespace xe {

XeEngine::XeEngine(int width, int height, std::string name) 
  : xeWindow{width, height, name}, xeDevice{xeWindow}, xeRenderer{xeWindow, xeDevice} {
    loadDescriptors();
  };

XeEngine::~XeEngine() {};

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

std::unique_ptr<XeRenderSystem> XeEngine::createRenderSystem(const std::string &vert, const std::string &frag, typename pushCunstant, typename uniformBuffer) {
  return std::make_unique<XeRenderSystem>(
    xeDevice,
    xeRenderer,
    xeDescriptorPool,
    xeDescriptorSetLayout,
    vert,
    frag,
    pushCunstantDataSize,
    uniformBufferDataSize
  );
}

}