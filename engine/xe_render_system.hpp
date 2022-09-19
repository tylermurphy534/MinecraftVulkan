#pragma once

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_frame_info.hpp"
#include "xe_game_object.hpp"

#include <memory>

namespace xe {

template<typename P, typename U>
class XeRenderSystem {
  public:

    struct XeData{};
    
    XeRenderSystem(
      XeDevice &device, 
      XeRenderer &renderer, 
      std::unique_ptr<XeDescriptorPool> &xeDescriptorPool, 
      std::unique_ptr<XeDescriptorSetLayout> &xeDescriptorSetLayout,
      std::string vert,
      std::string frag, 
      uint32_t pushCunstantDataSize, 
      uint32_t uniformBufferDataSize);

    ~XeRenderSystem();

    XeRenderSystem(const XeRenderSystem &) = delete;
    XeRenderSystem operator=(const XeRenderSystem &) = delete;

    void renderGameObjects(XeFrameInfo &frameInfo, std::vector<XeGameObject> &gameObjects, XeRenderSystem::XeData pushConstantData);

  private:
    void createUniformBuffers(std::unique_ptr<XeDescriptorPool> &xeDescriptorPool, std::unique_ptr<XeDescriptorSetLayout> &xeDescriptorSetLayout, uint32_t uniformBufferDataSize);
    void createPipelineLayout(std::unique_ptr<XeDescriptorSetLayout> &xeDescriptorSetLayout, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize);
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag);

    XeDevice& xeDevice;

    std::unique_ptr<XePipeline> xePipeline;
    std::vector<std::unique_ptr<XeBuffer>> uboBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    VkPipelineLayout pipelineLayout;

};

}