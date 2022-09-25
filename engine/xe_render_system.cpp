#include "xe_render_system.hpp"

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_game_object.hpp"
#include "xe_swap_chain.hpp"
#include "xe_renderer.hpp"
#include "xe_descriptors.hpp"
#include "xe_engine.hpp"

#include <memory>
#include <stdexcept>

#include <iostream>

namespace xe {

RenderSystem::RenderSystem(
  Engine &xeEngine,
  std::string vert,
  std::string frag,
  std::map<uint32_t, uint32_t> uniformBindings,
  std::map<uint32_t, Image*> imageBindings,
  uint32_t pushCunstantDataSize,
  bool cullingEnabled,
  std::vector<VkVertexInputAttributeDescription> attributeDescptions,
  uint32_t vertexSize
) : xeDevice{xeEngine.xeDevice}, 
    xeRenderer{xeEngine.xeRenderer},
    xeDescriptorPool{xeEngine.xeDescriptorPool},
    pushCunstantDataSize{pushCunstantDataSize},
    uniformBindings{uniformBindings},
    imageBindings{imageBindings} {
  createTextureSampler();
  createDescriptorSetLayout();
  createUniformBuffers();
  createDescriptorSets();
  createPipelineLayout();
  createPipeline(xeRenderer.getSwapChainRenderPass(), vert, frag, cullingEnabled, attributeDescptions, vertexSize);
}

RenderSystem::~RenderSystem() {
  vkDestroyPipelineLayout(xeDevice.device(), pipelineLayout, nullptr);
  vkDestroySampler(xeDevice.device(), textureSampler, nullptr);
};

void RenderSystem::createTextureSampler() {
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  if (vkCreateSampler(xeDevice.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void RenderSystem::createDescriptorSetLayout() {
  DescriptorSetLayout::Builder builder{xeDevice};
  
  for ( const auto &[binding, size]: uniformBindings) {
    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
  }

  for ( const auto &[binding, image]: imageBindings) {
    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, &textureSampler);
  }

  xeDescriptorSetLayout = builder.build();
}

void RenderSystem::createUniformBuffers() {
  for ( const auto &[binding, bufferSize]: uniformBindings) {
    uboBuffers[binding] = std::vector<std::unique_ptr<Buffer>>(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers[binding].size(); i++) {
      uboBuffers[binding][i] = std::make_unique<Buffer>(
        xeDevice,
        bufferSize,
        SwapChain::MAX_FRAMES_IN_FLIGHT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      uboBuffers[binding][i]->map();
    }
  }
}

void RenderSystem::createDescriptorSets() {

  descriptorSets = std::vector<VkDescriptorSet>(SwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < descriptorSets.size(); i++) {
    updateDescriptorSet(i, true);
  }

}

void RenderSystem::updateDescriptorSet(int frameIndex, bool allocate) {

  DescriptorWriter writer{*xeDescriptorSetLayout, *xeDescriptorPool};

  std::vector<VkDescriptorBufferInfo> bufferInfos{};

  int i = 0;
  for ( const auto &[binding, size]: uniformBindings) {
    bufferInfos.push_back(uboBuffers[binding][frameIndex]->descriptorInfo());
    writer.writeBuffer(binding, &bufferInfos[i]);
    i++;
  }

  for ( const auto &[binding, image]: imageBindings) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = image->textureImageView;
    imageInfo.sampler = textureSampler;
    writer.writeImage(binding, &imageInfo);
  }

  if (allocate) {
    writer.build(descriptorSets[frameIndex]);
  } else {
    writer.overwrite(descriptorSets[frameIndex]);
  }
    
}


void RenderSystem::createPipelineLayout() {

  VkPushConstantRange pushConstantRange;
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = pushCunstantDataSize;

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{xeDescriptorSetLayout->getDescriptorSetLayout()};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

  if (pushCunstantDataSize > 0) {
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  } else {
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
  }

  if(vkCreatePipelineLayout(xeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    std::runtime_error("failed to create pipeline layout!");
  }

}


void RenderSystem::createPipeline(VkRenderPass renderPass, std::string vert, std::string frag, bool cullingEnabled, std::vector<VkVertexInputAttributeDescription> attributeDescptions, uint32_t vertexSize) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  Pipeline::defaultPipelineConfigInfo(pipelineConfig);
  if (cullingEnabled) {
    pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  }
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  xePipeline = std::make_unique<Pipeline>(
    xeDevice,
    vert,
    frag,
    pipelineConfig,
    attributeDescptions,
    vertexSize
  );
}

void RenderSystem::start() {
  xeRenderer.beginSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
  xePipeline->bind(xeRenderer.getCurrentCommandBuffer());
  if(descriptorSets.size() > 0) {

    vkCmdBindDescriptorSets(
        xeRenderer.getCurrentCommandBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &descriptorSets[xeRenderer.getFrameIndex()],
        0,
        nullptr);

  }
}

void RenderSystem::loadPushConstant(void *pushConstantData) {
  vkCmdPushConstants(
        xeRenderer.getCurrentCommandBuffer(), 
        pipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
        0, 
        pushCunstantDataSize,
        pushConstantData);
}

void RenderSystem::loadUniformObject(uint32_t binding, void *uniformBufferData) {
  uboBuffers[binding][xeRenderer.getFrameIndex()]->writeToBuffer(uniformBufferData);
}

void RenderSystem::loadTexture(uint32_t binding, Image *image) {
  imageBindings[binding] = image;
  updateDescriptorSet(xeRenderer.getFrameIndex(), false);
}

void RenderSystem::render(GameObject &gameObject) {

  gameObject.model->bind(xeRenderer.getCurrentCommandBuffer());
  gameObject.model->draw(xeRenderer.getCurrentCommandBuffer());

}

void RenderSystem::stop() {
  xeRenderer.endSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
}

}