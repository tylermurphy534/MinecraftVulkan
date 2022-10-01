#pragma once

#include "xe_game_object.hpp"
#include "chunk_renderer.hpp"
#include "chunk.hpp"

#include <vector>

namespace app {

class World {

  public:

    World(xe::GameObject& viewer, int renderDistance, int worldSeed);
    ~World();

    void reloadChunks();
    void reloadChunks(int newRenderDistance);
    
    void render(xe::Camera& camera);

  private:

    void resetChunks();

    void unloadOldChunks();
    void loadNewChunks();
    void updateChunkMeshs();

    int viewX, viewZ;

    int worldSeed;
    int renderDistance;
    
    const xe::GameObject& viewer;
    std::vector<xe::GameObject> loadedChunks;

    ChunkRenderer chunkRenderer;

};

}