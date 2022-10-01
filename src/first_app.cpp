#include "first_app.hpp"

#include <chrono>
using namespace std::chrono;

namespace app {

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Minecraft Vulkan", "res/image/icon.png"} {};

FirstApp::~FirstApp() {}

void FirstApp::run() {

  Chunk::load();

  auto viewerObject = xe::GameObject::createGameObject();
  viewerObject.transform.translation = {0.f, 40.f, 0.f};
  viewerObject.transform.rotation.y = glm::radians(45.f);

  createGameObjects(viewerObject);

  SimpleRenderer renderer{xeEngine, Chunk::getTextures()};

  xe::Sound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  KeyboardMovementController cameraController{xeEngine.getInput(), viewerObject};

  while (xeEngine.poll()) {

    float frameTime = xeEngine.getFrameTime();

    cameraController.update(frameTime);
    xeEngine.getCamera().setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    if(xeEngine.beginFrame()) {
      renderer.render(loadedChunks, xeEngine.getCamera());
      xeEngine.endFrame();
    }
    
    reloadLoadedChunks(viewerObject);

  }

  xeEngine.close();

  Chunk::unload();

}

void FirstApp::createGameObjects(xe::GameObject& viewer) {
  int width = 2*RENDER_DISTANCE+1;
  loadedChunks.clear();
  for(int32_t x = 0; x < width; x++) {
    for(int32_t z = 0; z < width; z++) {
      auto gameObject = xe::GameObject::createGameObject();
      gameObject.transform.translation = glm::vec3(0.f);
      loadedChunks.push_back(std::move(gameObject));
    }
  }
}

void FirstApp::reloadLoadedChunks(xe::GameObject& viewer) {
  viewX = static_cast<int>(floor(viewer.transform.translation.x / Chunk::CHUNK_SIZE.x));
  viewZ = static_cast<int>(floor(viewer.transform.translation.z / Chunk::CHUNK_SIZE.z));
  int width = 2*RENDER_DISTANCE+1;
  int minX = viewX - RENDER_DISTANCE;
  int minZ = viewZ - RENDER_DISTANCE;
  int maxX = viewX + RENDER_DISTANCE;
  int maxZ = viewZ + RENDER_DISTANCE;
  for(int32_t x = 0; x < width; x++) {
    for(int32_t z = 0; z < width; z++) {
      auto& gameObject = loadedChunks[x + z * width];
      int gridX = static_cast<int>(floor(gameObject.transform.translation.x / Chunk::CHUNK_SIZE.x));
      int gridZ = static_cast<int>(floor(gameObject.transform.translation.z / Chunk::CHUNK_SIZE.z));
      int newGridX = minX + x;
      int newGridZ = minZ + z;
      if(gridX < minX || gridZ < minZ || gridX > maxX || gridZ > maxZ)
        Chunk::deleteChunk(gridX, gridZ);
      Chunk* chunk = Chunk::getChunk(newGridX, newGridZ);
      if(chunk == nullptr) {
        chunk = Chunk::newChunk(newGridX, newGridZ, 12345);
        Chunk::generateAsync(chunk);
      }
      if(chunk->getMesh() == nullptr){
        Chunk::createMeshAsync(chunk);
      }
      gameObject.model = chunk->getMesh();
      gameObject.transform.translation = glm::vec3(newGridX * Chunk::CHUNK_SIZE.x, 0, newGridZ * Chunk::CHUNK_SIZE.z);
    }
  }
}

}