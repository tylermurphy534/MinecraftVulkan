#include "first_app.hpp"

#include "xe_camera.hpp"
#include "xe_engine.hpp"
#include "xe_game_object.hpp"
#include "xe_model.hpp"
#include "xe_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_renderer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <array>

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Hello, Vulkan!"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  SimpleRenderer renderer{xeEngine};
    
  auto viewerObject = xe::XeGameObject::createGameObject();
  viewerObject.transform.translation = {-7.f, 3.f, -7.f};
  viewerObject.transform.rotation.y = glm::radians(45.f);
  KeyboardMovementController cameraController{};

  while (xeEngine.poll()) {

    float frameTime = xeEngine.getFrameTime();

    cameraController.moveInPlaneXZ(xeEngine.getWindow().getGLFWwindow(), frameTime, viewerObject);
    xeEngine.getCamera().setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    if(xeEngine.beginFrame()) {
      renderer.render(gameObjects, xeEngine.getCamera());
      xeEngine.endFrame();
    }
  }

  xeEngine.close();

}

void FirstApp::loadGameObjects() {
  std::shared_ptr<xe::XeModel> xeModel = xeEngine.loadModelFromFile("res/models/stanford-dragon.obj");

  auto dragon = xe::XeGameObject::createGameObject();
  dragon.model = xeModel;
  dragon.transform.translation = {.0f, .0f, 2.5f};
  dragon.transform.scale = {.5f, .5f, .5f};
  gameObjects.push_back(std::move(dragon));

  auto dragon2 = xe::XeGameObject::createGameObject();
  dragon2.model = xeModel;
  dragon2.transform.translation = {5.0f, .0f, -1.5f};
  dragon2.transform.rotation.y = glm::radians(90.f);
  dragon2.transform.scale = {.35f, .35f, .35f};
  gameObjects.push_back(std::move(dragon2));
}

}