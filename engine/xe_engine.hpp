#pragma once

#include "xe_window.hpp"
#include "xe_device.hpp"
#include "xe_renderer.hpp"
#include "xe_render_system.hpp"
#include "xe_descriptors.hpp"

#include <string>
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

    XeCamera& getCamera() { 
      return xeCamera; 
    };

    std::unique_ptr<XeRenderSystem> createRenderSystem(const std::string &vert, const std::string &frag, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize);

    std::shared_ptr<XeModel> createModel(const std::string &filename);

    bool beginFrame() { return xeRenderer.beginFrame(); }

    void endFrame() { return xeRenderer.endFrame(); }

    void render(
      XeRenderSystem& xeRenderSystem, 
      std::vector<XeGameObject>& gameObjects,
      void* pushConstantData, 
      uint32_t pushConstantSize, 
      void* uniformBufferData, 
      uint32_t uniformBufferSize);

    void close() { vkDeviceWaitIdle(xeDevice.device()); }

  private:

    void loadDescriptors();

    XeWindow xeWindow;
    XeDevice xeDevice;
    XeRenderer xeRenderer;
    XeCamera xeCamera;

    std::unique_ptr<XeDescriptorPool> xeDescriptorPool;
    std::unique_ptr<XeDescriptorSetLayout> xeDescriptorSetLayout;
};

}