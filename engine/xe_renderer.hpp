#pragma once

#include "xe_swap_chain.hpp"
#include "xe_window.hpp"
#include "xe_device.hpp"
#include "xe_model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace xe {
class XeRenderer {
  public:

    XeRenderer(XeWindow &window, XeDevice &device);
    ~XeRenderer();

    XeRenderer(const XeRenderer &) = delete;
    XeRenderer operator=(const XeRenderer &) = delete;

    VkRenderPass getSwapChainRenderPass() const { return xeSwapChain->getRenderPass(); }
    float getAspectRatio() const { return xeSwapChain->extentAspectRatio(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const { 
      assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
      return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const {
      assert(isFrameStarted && "Cannot get frame index when frame not in progress");
      return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    XeWindow& xeWindow;
    XeDevice& xeDevice;
    std::unique_ptr<XeSwapChain> xeSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
};
}