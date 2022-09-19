#pragma once

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_frame_info.hpp"
#include "xe_game_object.hpp"
#include "xe_descriptors.hpp"
#include "xe_renderer.hpp"

#include <memory>

namespace xe {

class XeRenderSystem {
  public:

    struct XeData{};
    
    XeRenderSystem(
      XeDevice &device, 
      XeRenderer &renderer, 
      XeDescriptorPool &xeDescriptorPool, 
      XeDescriptorSetLayout &xeDescriptorSetLayout,
      std::string vert,
      std::string frag, 
      uint32_t pushCunstantDataSize,
      uint32_t uniformBufferDataSize
      );

    ~XeRenderSystem();

    XeRenderSystem(const XeRenderSystem &) = delete;
    XeRenderSystem operator=(const XeRenderSystem &) = delete;

    void setUnifroms(XeFrameInfo & frameInfo);

    void renderGameObjects(
      int frameIndex, 
      VkCommandBuffer commandBuffer, 
      std::vector<XeGameObject> &gameObjects, 
      void *pushConstantData, 
      uint32_t pushConstantSize, 
      void* uniformBufferData, 
      uint32_t uniformBufferSize);

  private:
    void createUniformBuffers(XeDescriptorPool &xeDescriptorPool, XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t uniformBufferDataSize);
    void createPipelineLayout(XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize);
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag);

    XeDevice& xeDevice;

    std::unique_ptr<XePipeline> xePipeline;
    std::vector<std::unique_ptr<XeBuffer>> uboBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    VkPipelineLayout pipelineLayout;

};

}