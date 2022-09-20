#pragma once

#include "xe_engine.hpp"
#include "xe_game_object.hpp"
#include "xe_descriptors.hpp"

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

    xe::XeEngine xeEngine;

    std::vector<xe::XeGameObject> gameObjects;
};
}