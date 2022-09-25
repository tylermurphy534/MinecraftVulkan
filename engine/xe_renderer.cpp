#include "xe_renderer.hpp"

namespace xe {

Renderer::Renderer(Window& window, Device& device) : xeWindow{window}, xeDevice{device} {
  recreateSwapChain();
  createCommandBuffers();
}

Renderer::~Renderer() { freeCommandBuffers(); }

void Renderer::recreateSwapChain() { 
  auto extent = xeWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = xeWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(xeDevice.device());
  
  if(xeSwapChain == nullptr) {
    xeSwapChain = std::make_unique<SwapChain>(xeDevice, extent);
  } else {
    std::shared_ptr<SwapChain> oldSwapChain = std::move(xeSwapChain);
    xeSwapChain = std::make_unique<SwapChain>(xeDevice, extent, oldSwapChain);

    if(!oldSwapChain->compareSwapFormats(*xeSwapChain.get())) {
      throw std::runtime_error("Swap chain image (or depth) format has changed");
    }

  }

  // we'll come back to this in just a moment
}

void Renderer::createCommandBuffers() {
  
  commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = xeDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if(vkAllocateCommandBuffers(xeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

}

void Renderer::freeCommandBuffers() {
  vkFreeCommandBuffers(
    xeDevice.device(), 
    xeDevice.getCommandPool(), 
    static_cast<uint32_t>(commandBuffers.size()),
    commandBuffers.data());
    commandBuffers.clear();
}

VkCommandBuffer Renderer::beginFrame() {
  assert(!isFrameStarted && "Can't acll beingFrame while already in progress");

  auto result = xeSwapChain->acquireNextImage(&currentImageIndex);

  if(result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffers");
  }
  return commandBuffer;
}

void Renderer::endFrame() {
  assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
  auto commandBuffer = getCurrentCommandBuffer();
  if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }

  auto result = xeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || xeWindow.wasWindowResized()) {
    xeWindow.resetWindowResizedFlag();
    recreateSwapChain();
  }

  isFrameStarted = false;
  currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
  assert(isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = xeSwapChain->getRenderPass();
  renderPassInfo.framebuffer = xeSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = xeSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = static_cast<float>(xeSwapChain->getSwapChainExtent().height);
  viewport.width = static_cast<float>(xeSwapChain->getSwapChainExtent().width);
  viewport.height = -static_cast<float>(xeSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, xeSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
  assert(isFrameStarted && "Can't call endSwapChainRenderPass while frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);

}

}