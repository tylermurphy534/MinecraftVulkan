#pragma once

#include "xe_camera.hpp"
#include "xe_pipeline.hpp"
#include "xe_device.hpp"
#include "xe_game_object.hpp"
#include "xe_frame_info.hpp"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace xe {
class SimpleRenderSystem {
  public:

    SimpleRenderSystem(XeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(
      XeFrameInfo &frameInfo, 
      std::vector<XeGameObject> &gameObjects
    );

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    XeDevice& xeDevice;

    std::unique_ptr<XePipeline> xePipeline;
    VkPipelineLayout pipelineLayout;
};
}