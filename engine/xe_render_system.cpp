#include "xe_render_system.hpp"

namespace xe {

XeRenderSystem::XeRenderSystem(
  XeEngine &xeEngine,
  std::string vert,
  std::string frag,
  std::map<uint32_t, uint32_t> uniformBindings,
  std::map<uint32_t, XeImage*> imageBindings,
  uint32_t pushCunstantDataSize,
  bool cullingEnabled
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
  createPipeline(xeRenderer.getSwapChainRenderPass(), vert, frag, cullingEnabled);
}


XeRenderSystem::~XeRenderSystem() {
  vkDestroyPipelineLayout(xeDevice.device(), pipelineLayout, nullptr);
  vkDestroySampler(xeDevice.device(), textureSampler, nullptr);
};

void XeRenderSystem::createTextureSampler() {
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

void XeRenderSystem::createDescriptorSetLayout() {
  XeDescriptorSetLayout::Builder builder{xeDevice};
  
  for ( const auto &[binding, size]: uniformBindings) {
    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, nullptr);
  }

  for ( const auto &[binding, image]: imageBindings) {
    builder.addBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, &textureSampler);
  }

  xeDescriptorSetLayout = builder.build();
}

void XeRenderSystem::createUniformBuffers() {
  for ( const auto &[binding, bufferSize]: uniformBindings) {
    uboBuffers[binding] = std::vector<std::unique_ptr<XeBuffer>>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers[binding].size(); i++) {
      uboBuffers[binding][i] = std::make_unique<XeBuffer>(
        xeDevice,
        bufferSize,
        XeSwapChain::MAX_FRAMES_IN_FLIGHT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
      uboBuffers[binding][i]->map();
    }
  }
}

void XeRenderSystem::createDescriptorSets() {

  descriptorSets = std::vector<VkDescriptorSet>(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < descriptorSets.size(); i++) {
    updateDescriptorSet(i, true);
  }

}

void XeRenderSystem::updateDescriptorSet(int frameIndex, bool allocate) {

  XeDescriptorWriter writer{*xeDescriptorSetLayout, *xeDescriptorPool};

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


void XeRenderSystem::createPipelineLayout() {

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


void XeRenderSystem::createPipeline(VkRenderPass renderPass, std::string vert, std::string frag, bool cullingEnabled) {
  assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  XePipeline::defaultPipelineConfigInfo(pipelineConfig);
  if (cullingEnabled) {
    pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  }
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

void XeRenderSystem::loadUniformObject(uint32_t binding, void *uniformBufferData) {
  uboBuffers[binding][xeRenderer.getFrameIndex()]->writeToBuffer(uniformBufferData);
}

void XeRenderSystem::loadTexture(uint32_t binding, XeImage *image) {
  imageBindings[binding] = image;
  updateDescriptorSet(xeRenderer.getFrameIndex(), false);
}

void XeRenderSystem::render(XeGameObject &gameObject) {

  gameObject.model->bind(xeRenderer.getCurrentCommandBuffer());
  gameObject.model->draw(xeRenderer.getCurrentCommandBuffer());

}

void XeRenderSystem::stop() {
  xeRenderer.endSwapChainRenderPass(xeRenderer.getCurrentCommandBuffer());
}

}