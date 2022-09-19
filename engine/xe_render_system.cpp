#include "xe_render_system.hpp"

#include <vulkan/vulkan.h>
#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_frame_info.hpp"
#include "xe_game_object.hpp"
#include "xe_swap_chain.hpp"
#include "xe_renderer.hpp"
#include "xe_descriptors.hpp"

#include <memory>
#include <stdexcept>

#include <iostream>

namespace xe {

XeRenderSystem::XeRenderSystem(
  XeDevice &device, 
  XeRenderer &renderer, 
  XeDescriptorPool &xeDescriptorPool, 
  XeDescriptorSetLayout &xeDescriptorSetLayout, 
  std::string vert,
  std::string frag, 
  uint32_t pushCunstantDataSize, 
  uint32_t uniformBufferDataSize) 
  : xeDevice{device} {
  createUniformBuffers(xeDescriptorPool, xeDescriptorSetLayout, uniformBufferDataSize);
  createPipelineLayout(xeDescriptorSetLayout, pushCunstantDataSize, uniformBufferDataSize);
  createPipeline(renderer.getSwapChainRenderPass(), vert, frag);
}


XeRenderSystem::~XeRenderSystem() {};

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

void XeRenderSystem::renderGameObjects(
  int frameIndex, 
  VkCommandBuffer commandBuffer, 
  std::vector<XeGameObject> &gameObjects, 
  void *pushConstantData, 
  uint32_t pushConstantSize, 
  void* uniformBufferData, 
  uint32_t uniformBufferSize) {

  uboBuffers[frameIndex]->writeToBuffer(uniformBufferData);
  uboBuffers[frameIndex]->flush();

  xePipeline->bind(commandBuffer);

  if(pushConstantSize > 0) {
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &descriptorSets[frameIndex],
        0,
        nullptr);
  }
  
  for (auto& obj: gameObjects) {

    struct PushConstant {
      glm::mat4 modelMatrix; 
      glm::mat4 normalMatrix;
    };

    PushConstant pc = PushConstant{obj.transform.mat4(), obj.transform.normalMatrix()};

    if(pushConstantSize > 0) {
      vkCmdPushConstants(
        commandBuffer, 
        pipelineLayout, 
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
        0, 
        pushConstantSize,
        &pc);
    }

    obj.model->bind(commandBuffer);
    obj.model->draw(commandBuffer);
  }

}

}