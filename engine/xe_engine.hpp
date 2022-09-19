#pragma once

#include "xe_window.hpp"
#include "xe_device.hpp"
#include "xe_renderer.hpp"
#include "xe_render_system.hpp"
#include "xe_descriptors.hpp"

namespace xe {

class XeEngine {

  public:

    XeEngine(int width, int height, std::string name);
    ~XeEngine() {};

    XeEngine(const XeEngine&) = delete;
    XeEngine operator=(const XeEngine&) = delete;

    XeWindow& getWindow() { 
      return xeWindow; 
    };

    XeRenderer& getRenderer() { 
      return xeRenderer; 
    };

    std::unique_ptr<XeRenderSystem> createRenderSystem(const std::string &vert, const std::string &frag, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize);

  private:

    void loadDescriptors();

    XeWindow xeWindow;
    XeDevice xeDevice;
    XeRenderer xeRenderer;

    std::unique_ptr<XeDescriptorPool> xeDescriptorPool;
    std::unique_ptr<XeDescriptorSetLayout> xeDescriptorSetLayout;
};

}