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
  uint32_t uniformBufferDataSize) 
  : xeDevice{xeEngine.xeDevice}, xeRenderer{xeEngine.xeRenderer} {
  createUniformBuffers(*xeEngine.xeDescriptorPool, *xeEngine.xeDescriptorSetLayout, uniformBufferDataSize);
  createPipelineLayout(*xeEngine.xeDescriptorSetLayout, pushCunstantDataSize, uniformBufferDataSize);
  createPipeline(xeRenderer.getSwapChainRenderPass(), vert, frag);
}


XeRenderSystem::~XeRenderSystem() {
  vkDestroyPipelineLayout(xeDevice.device(), pipelineLayout, nullptr);
};

void XeRenderSystem::createUniformBuffers(XeDescriptorPool &xeDescriptorPool, XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t uniformBufferDataSize) {
  if(uniformBufferDataSize == 0) return;

  uboBuffers = std::vector<std::unique_ptr<XeBuffer>>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<XeBuffer>(
      xeDevice,
      uniformBufferDataSize,
      XeSwapChain::MAX_FRAMES_IN_FLIGHT,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  descriptorSets = std::vector<VkDescriptorSet>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < descriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    XeDescriptorWriter(xeDescriptorSetLayout, xeDescriptorPool)
      .writeBuffer(0, &bufferInfo)
      .build(descriptorSets[i]);
  }
}


void XeRenderSystem::createPipelineLayout(XeDescriptorSetLayout &xeDescriptorSetLayout, uint32_t pushCunstantDataSize, uint32_t uniformBufferDataSize) {

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

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{xeDescriptorSetLayout.getDescriptorSetLayout()};

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

void XeRenderSystem::loadPushConstant(void *pushConstantData, uint32_t pushConstantSize) {
  if(!boundPipeline) {
    xeRenderer.beginSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
    xePipeline->bind(xeRenderer.getCurrentCommandBuffer());
    boundPipeline = true;
  }
  if(!boundDescriptor) {
    vkCmdBindDescriptorSets(
        xeRenderer.getCurrentCommandBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &descriptorSets[xeRenderer.getFrameIndex()],
        0,
        nullptr);
    boundDescriptor = true;
  }
  vkCmdPushConstants(
        xeRenderer.getCurrentCommandBuffer(), 
        pipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
        0, 
        pushConstantSize,
        pushConstantData);
}

void XeRenderSystem::loadUniformObject(void *uniformBufferData, uint32_t uniformBufferSize) {
  uboBuffers[xeRenderer.getFrameIndex()]->writeToBuffer(uniformBufferData);
  uboBuffers[xeRenderer.getFrameIndex()]->flush();
}

void XeRenderSystem::render(XeGameObject &gameObject) {
  if(!boundPipeline){
    xeRenderer.beginSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
    xePipeline->bind(xeRenderer.getCurrentCommandBuffer());
    boundPipeline = true;
  }

  gameObject.model->bind(xeRenderer.getCurrentCommandBuffer());
  gameObject.model->draw(xeRenderer.getCurrentCommandBuffer());

}

void XeRenderSystem::stop() {
  boundPipeline = false;
  boundDescriptor = false;
  xeRenderer.endSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
}

}