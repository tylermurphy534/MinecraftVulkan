#pragma once

#include "xe_window.hpp"
#include "xe_device.hpp"
#include "xe_renderer.hpp"

namespace xe {

class XeEngine {

  public:

    XeEngine(int width, int height, std::string name);
    ~XeEngine() {};

    XeEngine(const XeEngine&) = delete;
    XeEngine operator=(const XeEngine&) = delete;

    const XeWindow& getWindow() const { 
      return xeWindow; 
    };

    const XeRenderer& getRenderer() const { 
      return xeRenderer; 
    };


  private:

    void loadDescriptorPool();

    XeWindow xeWindow;
    XeDevice xeDevice;
    XeRenderer xeRenderer;

    std::unique_ptr<XeDescriptorPool> xeDescriptorPool{};
    std::unique_ptr<xe::XeDescriptorSetLayout> xeDescriptorSetLayout;
};

}