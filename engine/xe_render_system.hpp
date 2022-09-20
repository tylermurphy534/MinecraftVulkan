#pragma once

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_game_object.hpp"
#include "xe_descriptors.hpp"
#include "xe_renderer.hpp"
#include "xe_engine.hpp"

#include <memory>

namespace xe {

class XeRenderSystem {
  public:
  
    XeRenderSystem(
      XeEngine &xeEngine,
      std::string vert,
      std::string frag, 
      uint32_t pushCunstantDataSize,
      uint32_t uniformBufferDataSize
    );

    ~XeRenderSystem();

    XeRenderSystem(const XeRenderSystem &) = delete;
    XeRenderSystem operator=(const XeRenderSystem &) = delete;

    void loadPushConstant(void *pushConstantData, uint32_t pushConstantSize);
    void loadUniformObject(void *uniformBufferData, uint32_t uniformBufferSize);
    void render(XeGameObject &gameObject);
    void stop();

  private:
  
    void createUniformBuffers(XeDescriptorPool &xeDescriptorPool, XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t uniformBufferDataSize);
    void createPipelineLayout(XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize);
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag);

    bool boundPipeline{false};
    bool boundDescriptor{false};

    XeDevice& xeDevice;
    XeRenderer& xeRenderer;

    std::unique_ptr<XePipeline> xePipeline;
    std::vector<std::unique_ptr<XeBuffer>> uboBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    VkPipelineLayout pipelineLayout;

};

}