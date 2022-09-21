#include "xe_engine.hpp"
#include "xe_image.hpp"
#include <chrono> 

namespace xe {

XeEngine::XeEngine(int width, int height, std::string name) : xeWindow{width, height, name}, 
  xeDevice{xeWindow}, 
  xeRenderer{xeWindow, xeDevice},
  xeCamera{} {
  loadDescriptorPool();
};

void XeEngine::loadDescriptorPool() {
  xeDescriptorPool = XeDescriptorPool::Builder(xeDevice)
    .setMaxSets(XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();
}

std::shared_ptr<XeModel> XeEngine::loadModelFromFile(const std::string &filename) {
  return XeModel::createModelFromFile(xeDevice, filename);
}

std::shared_ptr<XeModel> XeEngine::loadModelFromData(std::vector<XeModel::Vertex> vertices, std::vector<uint32_t> indices) {
  XeModel::Builder builder{};
  builder.vertices = vertices;
  if(&indices == NULL) { 
    builder.indices = indices;
  }
  return std::make_shared<XeModel>(xeDevice, builder);
}

std::shared_ptr<XeImage> XeEngine::loadImage(const std::string &filename) {
  return std::make_shared<XeImage>(xeDevice, filename);
}

bool XeEngine::poll() {
  glfwPollEvents();
  auto newTime = std::chrono::high_resolution_clock::now();
  frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
  currentTime = newTime;
  float aspect = xeRenderer.getAspectRatio();
  xeCamera.setPerspectiveProjection(glm::radians(FOV), aspect, 0.1f, 100.f);
  return !xeWindow.shouldClose();
}

}