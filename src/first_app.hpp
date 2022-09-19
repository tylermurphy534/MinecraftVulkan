#pragma once

#include "xe_engine.hpp"
#include "xe_game_object.hpp"
#include "xe_descriptors.hpp"

#include <memory>
#include <vector>

namespace xe {
class FirstApp {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp operator=(const FirstApp &) = delete;

    void run();

  private:
    void loadGameObjects();

    XeEngine xeEngine;

    std::vector<XeGameObject> gameObjects;
};
}