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
  alutInit(0, NULL);
  std::cout << "Audio device: " << alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER) << "\n";
  _instance = this;
};

Engine::~Engine() {
  Model::submitDeleteQueue(true);
  Image::submitDeleteQueue(true);
  alutExit();
};

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