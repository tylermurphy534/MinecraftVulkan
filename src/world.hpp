#pragma once

#include "xe_game_object.hpp"
#include "skinned_renderer.hpp"
#include "chunk.hpp"

#define GLM_FORCE_RADIANS
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include <vector>

namespace app {

class World {

  public:

    struct Ray {
      glm::ivec3 pos;
      int hit;
    };

    World(xe::GameObject& viewer, int renderDistance, int worldSeed);
    ~World();

    void reloadChunks();
    void reloadChunks(int newRenderDistance);
    
    void render(xe::Camera& camera);

    Ray raycast(float distance, int steps);

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

    SkinnedRenderer skinnedRenderer;

};

}