#pragma once

#include "xe_engine.hpp"

#include "player_controller.hpp"
#include "chunk_renderer.hpp"
#include "chunk.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <array>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

namespace app {
class Minecraft {
  public:

    Minecraft();
    ~Minecraft();

    Minecraft(const Minecraft &) = delete;
    Minecraft operator=(const Minecraft &) = delete;

    void run();

  private:
  
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    static constexpr int RENDER_DISTANCE = 10;

    void createGameObjects(xe::GameObject& viewer);
    void reloadLoadedChunks(xe::GameObject& viewer);
    

    int viewX, viewZ;

    xe::Engine xeEngine;
    std::vector<xe::GameObject> loadedChunks;
};
}