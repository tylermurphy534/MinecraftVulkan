#include "xe_engine.hpp"

namespace xe {

static Engine* _instance;

Engine* Engine::getInstance() {
  return _instance;
}

Engine::Engine(int width, int height, std::string name, const char *icon) : xeWindow{width, height, name, icon}, 
  xeDevice{xeWindow}, 
  xeRenderer{xeWindow, xeDevice},
  xeCamera{},
  xeInput{xeWindow} {
  loadDescriptorPool();
  alutInit(0, NULL);
  std::cout << "Audio device: " << alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER) << "\n";
  _instance = this;
};

Engine::~Engine() {
  alutExit();
};

void Engine::loadDescriptorPool() {
  xeDescriptorPool = DescriptorPool::Builder(xeDevice)
    .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();
}

Model* Engine::loadModelFromFile(const std::string &filename) {
  return Model::createModelFromFile(xeDevice, filename);
}

Model* Engine::loadModelFromData(std::vector<unsigned char> vertexData, uint32_t vertexSize, std::vector<uint32_t> indices) {
  Model::Builder builder{};
  builder.vertexData.data = vertexData;
  builder.vertexSize = vertexSize;
  if(indices.size() > 0) { 
    builder.indices = indices;
  }
  return new Model(xeDevice, builder);
}

Image* Engine::loadImageFromFile(const std::string &filename, bool anisotropic) {
  return new Image(xeDevice, filename, anisotropic);
}

bool Engine::poll() {
  glfwPollEvents();
  auto newTime = std::chrono::high_resolution_clock::now();
  frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
  currentTime = newTime;
  float aspect = xeRenderer.getAspectRatio();
  xeCamera.setPerspectiveProjection(glm::radians(FOV), aspect, 0.1f, 1000.f);
  return !xeWindow.shouldClose();
}

}