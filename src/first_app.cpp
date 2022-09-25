#include "first_app.hpp"

#include "xe_camera.hpp"
#include "xe_engine.hpp"
#include "xe_game_object.hpp"
#include "xe_model.hpp"
#include "xe_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_renderer.hpp"
#include "xe_sound.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <array>

#include <string>

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Xenon Vulkan Engine"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::shared_ptr<xe::Image> image = xeEngine.loadImage("res/image/texture.png"); 
  std::shared_ptr<xe::Image> image2 = xeEngine.loadImage("res/image/ltt."); 

  SimpleRenderer renderer{xeEngine, image.get()};

  xe::Sound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  auto viewerObject = xe::GameObject::createGameObject();
  viewerObject.transform.translation = {-7.f, 3.f, -7.f};
  viewerObject.transform.rotation.y = glm::radians(45.f);
  KeyboardMovementController cameraController{};

  while (xeEngine.poll()) {

    float frameTime = xeEngine.getFrameTime();

    cameraController.moveInPlaneXZ(xeEngine.getWindow().getGLFWwindow(), frameTime, viewerObject);
    xeEngine.getCamera().setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    if(xeEngine.beginFrame()) {
      renderer.render(gameObjects, xeEngine.getCamera(), image2.get());
      xeEngine.endFrame();
    }

  }

  xeEngine.close();

}

void FirstApp::loadGameObjects() {
  std::shared_ptr<xe::Model> xeModel = xeEngine.loadModelFromFile("res/models/stanford-dragon.obj");

  auto dragon = xe::GameObject::createGameObject();
  dragon.model = xeModel;
  dragon.transform.translation = {.0f, .0f, 2.5f};
  dragon.transform.scale = {.5f, .5f, .5f};
  gameObjects.push_back(std::move(dragon));

  auto dragon2 = xe::GameObject::createGameObject();
  dragon2.model = xeModel;
  dragon2.transform.translation = {5.0f, .0f, -1.5f};
  dragon2.transform.rotation.y = glm::radians(90.f);
  dragon2.transform.scale = {.35f, .35f, .35f};
  gameObjects.push_back(std::move(dragon2));
}

}