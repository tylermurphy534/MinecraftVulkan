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

XeRenderSystem::XeRenderSystem(
  XeEngine &xeEngine,
  std::string vert,
  std::string frag, 
  uint32_t pushCunstantDataSize, 
  uint32_t uniformBufferDataSize,
  XeImage *image
) : xeDevice{xeEngine.xeDevice}, 
    xeRenderer{xeEngine.xeRenderer},
    xeDescriptorPool{xeEngine.xeDescriptorPool},
    pushCunstantDataSize{pushCunstantDataSize},
    uniformBufferDataSize{uniformBufferDataSize},
    textureSamplerBinding{image != nullptr} {
  createDescriptorSetLayout();
  createUniformBuffers();
  createDescriptorSets(image);
  createPipelineLayout();
  createPipeline(xeRenderer.getSwapChainRenderPass(), vert, frag);
}


XeRenderSystem::~XeRenderSystem() {
  vkDestroyPipelineLayout(xeDevice.device(), pipelineLayout, nullptr);
  if ( textureSamplerBinding ) {
    vkDestroySampler(xeDevice.device(), textureSampler, nullptr);
  }
};

void XeRenderSystem::createDescriptorSetLayout() {
  XeDescriptorSetLayout::Builder builder{xeDevice};
  int binding = 0;

  if (uniformBufferDataSize > 0) {
    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
    binding += 1;
  }
  if (textureSamplerBinding) {

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

    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, &textureSampler);
    binding += 1;
  }

  xeDescriptorSetLayout = builder.build();
}

void XeRenderSystem::createUniformBuffers() {
  if(uniformBufferDataSize == 0) return;

  uboBuffers = std::vector<std::unique_ptr<XeBuffer>>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<XeBuffer>(
      xeDevice,
      uniformBufferDataSize,
      XeSwapChain::MAX_FRAMES_IN_FLIGHT,

      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    uboBuffers[i]->map();
  }

}

void XeRenderSystem::createDescriptorSets(XeImage *image) {

  descriptorSets = std::vector<VkDescriptorSet>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < descriptorSets.size(); i++) {
    updateDescriptorSet(image, i, true);
  }

}

void XeRenderSystem::updateDescriptorSet(XeImage *image, int frameIndex, bool allocate) {
  auto bufferInfo = uboBuffers[frameIndex]->descriptorInfo();
    XeDescriptorWriter writer{*xeDescriptorSetLayout, *xeDescriptorPool};
    
    int binding = 0;

    if (uniformBufferDataSize > 0) {
      writer.writeBuffer(binding, &bufferInfo);
      binding += 1;
    }
    
    if (textureSamplerBinding) {
      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = image->textureImageView;
      imageInfo.sampler = textureSampler;
      writer.writeImage(binding, &imageInfo);
      binding += 1;
    }
    if (allocate) {
      writer.build(descriptorSets[frameIndex]);
    } else {
      writer.overwrite(descriptorSets[frameIndex]);
    }
    
}


void XeRenderSystem::createPipelineLayout() {

  VkPushConstantRange pushConstantRange;
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = pushCunstantDataSize;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  if (pushCunstantDataSize > 0) {
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  } else {
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
  }

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{xeDescriptorSetLayout->getDescriptorSetLayout()};

  if (uniformBufferDataSize > 0) {
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  } else {

    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
  }

  if(vkCreatePipelineLayout(xeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    std::runtime_error("failed to create pipeline layout!");
  }

}


void XeRenderSystem::createPipeline(VkRenderPass renderPass, std::string vert, std::string frag) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  XePipeline::defaultPipelineConfigInfo(pipelineConfig);
  pipelineConfig.renderPass = renderPass;
  pipelineConfig.pipelineLayout = pipelineLayout;
  xePipeline = std::make_unique<XePipeline>(
    xeDevice,
    vert,
    frag,
    pipelineConfig
  );
}

void XeRenderSystem::start() {
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

void XeRenderSystem::loadPushConstant(void *pushConstantData) {
  vkCmdPushConstants(
        xeRenderer.getCurrentCommandBuffer(), 
        pipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
        0, 
        pushCunstantDataSize,
        pushConstantData);
}

void XeRenderSystem::loadUniformObject(void *uniformBufferData) {
  uboBuffers[xeRenderer.getFrameIndex()]->writeToBuffer(uniformBufferData);
}

void XeRenderSystem::loadTexture(XeImage *image) {
  updateDescriptorSet(image, xeRenderer.getFrameIndex(), false);
}

void XeRenderSystem::render(XeGameObject &gameObject) {

  gameObject.model->bind(xeRenderer.getCurrentCommandBuffer());
  gameObject.model->draw(xeRenderer.getCurrentCommandBuffer());

}

void XeRenderSystem::stop() {
  xeRenderer.endSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
}

}