#include "first_app.hpp"
#include "chunk.hpp"

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Xenon Vulkan Engine"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::shared_ptr<xe::Image> image = xeEngine.loadImageFromFile("res/image/dirt.jpg"); 

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
      renderer.render(gameObjects, xeEngine.getCamera(), image.get());
      xeEngine.endFrame();
    }

  }

  Chunk::reset();

  xeEngine.close();

}

void FirstApp::loadGameObjects() {

  Chunk* chunk = Chunk::newChunk(0, 0, 123);
  chunk->createMesh();

  auto chunkObject = xe::GameObject::createGameObject();
  chunkObject.model = chunk->getMesh();
  chunkObject.transform.translation = {0.f, 0.f, 0.f};
  gameObjects.push_back(std::move(chunkObject));

}

}