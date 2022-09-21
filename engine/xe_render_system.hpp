#pragma once

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_game_object.hpp"
#include "xe_descriptors.hpp"
#include "xe_renderer.hpp"
#include "xe_engine.hpp"
#include "xe_image.hpp"

#include <memory>

namespace xe {

class XeRenderSystem {
  public:
  
    XeRenderSystem(
      XeEngine &xeEngine,
      std::string vert,
      std::string frag, 
      uint32_t pushCunstantDataSize,
      uint32_t uniformBufferDataSize,
      XeImage *image
    );

    ~XeRenderSystem();

    XeRenderSystem(const XeRenderSystem &) = delete;
    XeRenderSystem operator=(const XeRenderSystem &) = delete;

    void start();
    void loadPushConstant(void *pushConstantData);
    void loadUniformObject(void *uniformBufferData);
    void loadTexture(XeImage *image);
    void render(XeGameObject &gameObject);
    void stop();

  private:
  
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createTextureImageView(XeImage *image);
    void createDescriptorSets(XeDescriptorPool &xeDescriptorPool);
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag);

    bool boundPipeline{false};
    bool boundDescriptor{false};

    uint32_t uniformBufferDataSize;
    uint32_t pushCunstantDataSize;
    bool textureSamplerBinding;

    XeDevice& xeDevice;
    XeRenderer& xeRenderer;

    std::unique_ptr<XePipeline> xePipeline;
    std::vector<std::unique_ptr<XeBuffer>> uboBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    VkSampler textureSampler;
    VkImageView textureImageView;
    
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<XeDescriptorSetLayout> xeDescriptorSetLayout;

};

}