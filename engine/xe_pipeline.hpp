#pragma once

#include "xe_device.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace xe {

struct PipelineConfigInfo {
  PipelineConfigInfo(const PipelineConfigInfo&) = delete;
  PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class Pipeline {
  public:
    Pipeline(
      Device &device, 
      const std::string& vertFilepath, 
      const std::string& fragFilepath, 
      const PipelineConfigInfo& configInfo,
      std::vector<VkVertexInputAttributeDescription> &attributeDescptions,
      uint32_t vertexSize  
    );
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline operator=(const Pipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

  private:
    static std::vector<char> readFile(const std::string& filePath);

    void createGraphicsPipeline(
      const std::string& vertFilePath, 
      const std::string& fragFilepath, 
      const PipelineConfigInfo& configInfo,
      std::vector<VkVertexInputAttributeDescription> &attributeDescptions,
      uint32_t vertexSize
    );

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    Device& xeDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};

}