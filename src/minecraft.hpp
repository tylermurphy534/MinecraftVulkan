#pragma once

#include "xe_engine.hpp"

#include "player_controller.hpp"
#include "chunk.hpp"
#include "world.hpp"

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

    void run();

  private:
  
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    xe::Engine engine;
};
}