#include "first_app.hpp"
#include "chunk.hpp"

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Xenon Vulkan Engine"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::shared_ptr<xe::Image> image = xeEngine.loadImageFromFile("res/image/texture.png"); 
  std::shared_ptr<xe::Image> image2 = xeEngine.loadImageFromFile("res/image/scaly.png"); 

  SimpleRenderer renderer{xeEngine, image.get()};

  xe::Sound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  auto viewerObject = xe::GameObject::createGameObject();
  viewerObject.transform.translation = {-7.f, 3.f, -7.f};
  viewerObject.transform.rotation.y = glm::radians(45.f);
  KeyboardMovementController cameraController{xeEngine.getInput(), viewerObject};

  while (xeEngine.poll()) {

    float frameTime = xeEngine.getFrameTime();

    cameraController.update(frameTime);
    xeEngine.getCamera().setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    if(xeEngine.beginFrame()) {
      renderer.render(gameObjects, xeEngine.getCamera(), image2.get());
      xeEngine.endFrame();
    }

  }

  Chunk::reset();

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

  Chunk* chunk = Chunk::newChunk(0, 0, 123);
  chunk->createMesh();

  auto chunkObject = xe::GameObject::createGameObject();
  chunkObject.model = chunk->getMesh();
  chunkObject.transform.translation = {5.f, 5.f, 5.f};
  gameObjects.push_back(std::move(chunkObject));

}

}