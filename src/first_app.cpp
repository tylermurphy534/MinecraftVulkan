#include "first_app.hpp"

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Xenon Vulkan Engine"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::shared_ptr<xe::XeImage> image = xeEngine.loadImage("res/image/texture.png"); 
  std::shared_ptr<xe::XeImage> image2 = xeEngine.loadImage("res/image/scaly.png"); 

  SimpleRenderer renderer{xeEngine, image.get()};

  xe::XeSound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  auto viewerObject = xe::XeGameObject::createGameObject();
  viewerObject.transform.translation = {-7.f, 3.f, -7.f};
  viewerObject.transform.rotation.y = glm::radians(45.f);
  KeyboardMovementController cameraController{};

  while (xeEngine.poll()) {

    float frameTime = xeEngine.getFrameTime();

    cameraController.update(xeEngine.getInput(), viewerObject, frameTime);
    xeEngine.getCamera().setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    if(xeEngine.beginFrame()) {
      renderer.render(gameObjects, xeEngine.getCamera(), image2.get());
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