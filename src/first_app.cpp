#include "first_app.hpp"
#include "chunk.hpp"

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Xenon Vulkan Engine"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::shared_ptr<xe::Image> dirt = xeEngine.loadImageFromFile("res/image/dirt.jpg"); 
  std::shared_ptr<xe::Image> grass = xeEngine.loadImageFromFile("res/image/grass.png"); 

  std::vector<xe::Image*> images = {dirt.get(), grass.get()};

  SimpleRenderer renderer{xeEngine, images};

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
      renderer.render(gameObjects, xeEngine.getCamera());
      xeEngine.endFrame();
    }

  }

  Chunk::reset();

  xeEngine.close();

}

void FirstApp::loadGameObjects() {

  for(int32_t x = 0; x < 10; x++) {
    for(int32_t z = 0; z < 10; z++) {
      Chunk* chunk = Chunk::newChunk(x, z, 53463);
    }
  }

  for(int32_t x = 0; x < 10; x++) {
    for(int32_t z = 0; z < 10; z++) {
      Chunk* chunk = Chunk::getChunk(x,z);
      chunk->createMesh();
      
      auto chunkObject = xe::GameObject::createGameObject();
      chunkObject.model = chunk->getMesh();
      chunkObject.transform.translation = {16.f*x, 0.f, 16.f*z};
      gameObjects.push_back(std::move(chunkObject));
    }
  }

}

}