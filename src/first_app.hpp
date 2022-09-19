#pragma once

#include "xe_renderer.hpp"
#include "xe_window.hpp"
#include "xe_device.hpp"
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

    XeWindow xeWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    XeDevice xeDevice{xeWindow};
    XeRenderer xeRenderer{xeWindow, xeDevice};

    std::unique_ptr<XeDescriptorPool> globalPool{};
    std::vector<XeGameObject> gameObjects;
};
}