#include "world.hpp"

namespace app {

World::World(xe::GameObject& viewer, int renderDistance, int worldSeed) 
  : viewer{viewer}, 
    renderDistance{renderDistance},
    worldSeed{worldSeed},
    skinnedRenderer{Chunk::getTextures()} {
  reloadChunks(renderDistance);
}

World::~World() {}

void World::reloadChunks() {
  int currentViewX = static_cast<int>(floor(viewer.transform.translation.x / Chunk::CHUNK_SIZE.x));
  int currentViewZ = static_cast<int>(floor(viewer.transform.translation.z / Chunk::CHUNK_SIZE.z));
  if(currentViewX != viewX || currentViewZ != viewZ) {
    viewX = currentViewX;
    viewZ = currentViewZ;
    unloadOldChunks();
    loadNewChunks();
  }
  updateChunkMeshs();
}

void World::reloadChunks(int newRenderDistance) {
  renderDistance = newRenderDistance;
  viewX = static_cast<int>(floor(viewer.transform.translation.x / Chunk::CHUNK_SIZE.x));
  viewZ = static_cast<int>(floor(viewer.transform.translation.z / Chunk::CHUNK_SIZE.z));
  resetChunks();
  loadNewChunks();
  updateChunkMeshs();
}

void World::resetChunks() {
  unloadOldChunks();
  loadedChunks.clear();
  int width = 2*renderDistance+1;
  for(int i = 0; i < width*width; i++) {
    auto gameObject = xe::GameObject::createGameObject();
    loadedChunks.push_back(std::move(gameObject));
  }
}

void World::unloadOldChunks() {
  int minX = viewX - renderDistance;
  int minZ = viewZ - renderDistance;
  int maxX = viewX + renderDistance;
  int maxZ = viewZ + renderDistance;
  for(auto &object : loadedChunks) {
    int gridX = static_cast<int>(floor(object.transform.translation.x / Chunk::CHUNK_SIZE.x));
    int gridZ = static_cast<int>(floor(object.transform.translation.z / Chunk::CHUNK_SIZE.z));
    if(gridX < minX || gridZ < minZ || gridX > maxX || gridZ > maxZ){
        Chunk::deleteChunk(gridX, gridZ);
        object.model = nullptr;
    }
  }
}

void World::loadNewChunks() {
  int width = 2*renderDistance+1;
  int minX = viewX - renderDistance;
  int minZ = viewZ - renderDistance;
  int maxX = viewX + renderDistance;
  int maxZ = viewZ + renderDistance;
  int i = 0;
  for(auto &object : loadedChunks) {
    int gridX = minX + i % width;
    int gridZ = minZ + i / width;
    Chunk* chunk = Chunk::getChunk(gridX, gridZ);
    if(chunk == nullptr) {
      chunk = Chunk::newChunk(gridX, gridZ, worldSeed);
      Chunk::generateAsync(chunk);
    }
    object.transform.translation = glm::vec3(gridX * Chunk::CHUNK_SIZE.x, 0, gridZ * Chunk::CHUNK_SIZE.z);
    i++;
  }
}

void World::updateChunkMeshs() {
  for(auto &object : loadedChunks) {
    int gridX = static_cast<int>(floor(object.transform.translation.x / Chunk::CHUNK_SIZE.x));
    int gridZ = static_cast<int>(floor(object.transform.translation.z / Chunk::CHUNK_SIZE.z));
    Chunk* chunk = Chunk::getChunk(gridX, gridZ);
    if(chunk == nullptr) continue;
    if(chunk->getMesh() == nullptr)
      Chunk::createMeshAsync(chunk);
    object.model = chunk->getMesh();
  }
}

void World::render(xe::Camera& camera) {
  camera.setViewYXZ(viewer.transform.translation, viewer.transform.rotation);
  // World::Ray ray = raycast(7, 100);
  skinnedRenderer.render(loadedChunks, camera);
}

World::Ray World::raycast(float distance, int steps) {

  glm::vec3 position = glm::vec3(viewer.transform.translation);

  float pitch = viewer.transform.rotation.x;
  float yaw = viewer.transform.rotation.y;
  float clamp = 1-fabs(sin(-pitch));
  const glm::vec3 step = glm::normalize(glm::vec3(sin(yaw)*clamp, sin(-pitch), cos(yaw)*clamp)) * (distance/steps);

  for(int i = 0; i < steps; i++) {
    position += step;
    int hit = Chunk::getGlobalBlock(position.x, position.y, position.z);
    if(hit == AIR) continue;
    return World::Ray{position, hit};
  }
  return World::Ray{position, INVALID};
}

}