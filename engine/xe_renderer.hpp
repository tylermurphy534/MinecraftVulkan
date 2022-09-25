#pragma once

#include "xe_device.hpp"
#include "xe_game_object.hpp"
#include "xe_swap_chain.hpp"
#include "xe_descriptors.hpp"
#include "xe_window.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace xe {
class Renderer {
  public:

    Renderer(Window &window, Device &device);
    ~Renderer();

    Renderer(const Renderer &) = delete;
    Renderer operator=(const Renderer &) = delete;

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

    Window& xeWindow;
    Device& xeDevice;
    std::unique_ptr<SwapChain> xeSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};
};
}