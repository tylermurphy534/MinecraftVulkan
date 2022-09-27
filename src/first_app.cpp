#include "first_app.hpp"
#include "chunk.hpp"

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Minecraft Vulkan", "res/image/icon.png"} {};

FirstApp::~FirstApp() {}

void FirstApp::run() {

  Chunk::load();

  for(int32_t x = 0; x < 10; x++) {
    for(int32_t z = 0; z < 10; z++) {
      Chunk* chunk = Chunk::newChunk(x, z, 53463);
      Chunk::createMeshAsync(chunk);
      auto chunkObject = xe::GameObject::createGameObject();
      chunkObject.transform.translation = {16.f*x, 0.f, 16.f*z};
      gameObjects.push_back(std::move(chunkObject));
    }
  }

  SimpleRenderer renderer{xeEngine, Chunk::getTextures()};

  xe::Sound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  auto viewerObject = xe::GameObject::createGameObject();
  viewerObject.transform.translation = {0.f, 10.f, 0.f};
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

  xeEngine.close();

  Chunk::unload();

}

}