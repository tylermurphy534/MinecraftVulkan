#include "xe_engine.hpp"
#include "xe_descriptors.hpp"

namespace xe {

XeEngine::XeEngine(int width, int height, std::string name) 
  : xeWindow{width, height, name}, xeDevice{xeWindow}, xeRenderer{xeWindow, xeDevice} {};

void XeEngine::loadDescriptorPool() {
  xeDescriptorPool = XeDescriptorPool::Builder(xeDevice)
    .setMaxSets(XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();
}

}