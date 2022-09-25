#pragma once

#include "xe_engine.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_renderer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <array>
#include <string>
#include <memory>
#include <vector>

namespace app {
class FirstApp {
  public:

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp operator=(const FirstApp &) = delete;

    void run();

  private:
  
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void loadGameObjects();

    xe::Engine xeEngine;

    std::vector<xe::GameObject> gameObjects;
};
}