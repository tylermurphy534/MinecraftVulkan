#pragma once

#include "xe_device.hpp"
#include "xe_pipeline.hpp"
#include "xe_game_object.hpp"
#include "xe_swap_chain.hpp"
#include "xe_renderer.hpp"
#include "xe_descriptors.hpp"
#include "xe_engine.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <map>
#include <vector>

namespace xe {

class RenderSystem {
  public:
  
    class Builder {
      public:
        Builder(std::string vert, std::string frag) : vert{vert}, frag{frag} {}

        Builder& addVertexBindingf(uint32_t binding, uint32_t dimension, uint32_t offset){
          if(dimension == 1)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32_SFLOAT, offset});
          if(dimension == 2)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32G32_SFLOAT, offset});
          if(dimension == 3)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32G32B32_SFLOAT, offset});
          return *this;
        }

        Builder& addVertexBindingi(uint32_t binding, uint32_t dimension, uint32_t offset){
          if(dimension == 1)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32_SINT, offset});
          if(dimension == 2)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32G32_SINT, offset});
          if(dimension == 3)
            attributeDescptions.push_back({binding, 0, VK_FORMAT_R32G32B32_SINT, offset});
          return *this;
        }

        Builder& setVertexSize(uint32_t size) {
          vertexSize = size;
          return *this;
        }

        Builder& addPushConstant(uint32_t size) {
          pushCunstantDataSize = size;
          return *this;
        }

        Builder& addUniformBinding(uint32_t binding, uint32_t size) {
          uniformBindings[binding] = size;
          return *this;
        }

        Builder& addTextureBinding(uint32_t binding, Image* image) {
          imageBindings[binding] = image;
          return *this;
        }

        Builder& addTextureArrayBinding(uint32_t binding, std::vector<Image*>& image) {
          imageArrayBindings[binding] = image;
          return *this;
        }

        Builder& setCulling(bool enabled) {
          cullingEnabled = enabled;
          return *this;
        }

        std::unique_ptr<RenderSystem> build() {
          return std::make_unique<RenderSystem>(std::move(vert), std::move(frag), std::move(uniformBindings), std::move(imageBindings), std::move(imageArrayBindings), std::move(pushCunstantDataSize), std::move(cullingEnabled), std::move(attributeDescptions), std::move(vertexSize));
        }

      private:

        std::map<uint32_t, uint32_t> uniformBindings{};
        std::map<uint32_t, Image*> imageBindings{};
        std::map<uint32_t, std::vector<Image*>> imageArrayBindings{};
        uint32_t pushCunstantDataSize{0};

        std::vector<VkVertexInputAttributeDescription> attributeDescptions{};
        uint32_t vertexSize;

        std::string vert;
        std::string frag;

        bool cullingEnabled{false};
    };

    RenderSystem(
      std::string vert,
      std::string frag,
      std::map<uint32_t, uint32_t> uniformBindings,
      std::map<uint32_t, Image*> imageBindings,
      std::map<uint32_t, std::vector<Image*>> imageArrayBindings,
      uint32_t pushCunstantDataSize,
      bool cullingEnabled,
      std::vector<VkVertexInputAttributeDescription> attributeDescptions,
      uint32_t vertexSize
    );

    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem operator=(const RenderSystem &) = delete;

    void start();
    void loadPushConstant(void *pushConstantData);
    void loadUniformObject(uint32_t binding, void *uniformBufferData);
    void loadTexture(uint32_t binding, Image *image);
    void loadTextureArray(uint32_t binding, std::vector<Image*> &images);
    void render(GameObject &gameObject);
    void stop();

  private:
  
    void createDescriptorPool();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorSets();
    void updateDescriptorSet(int frameIndex, bool allocate);
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag, bool cullingEnabled, std::vector<VkVertexInputAttributeDescription> attributeDescptions, uint32_t vertexSize);

    bool boundPipeline{false};
    bool boundDescriptor{false};

  
    Device& xeDevice;
    Renderer& xeRenderer;

    std::map<uint32_t, std::vector<std::unique_ptr<Buffer>>> uboBuffers{};
    std::map<uint32_t, uint32_t> uniformBindings;
    std::map<uint32_t, Image*> imageBindings;
    std::map<uint32_t, std::vector<Image*>> imageArrayBindings{};
    std::vector<VkDescriptorSet> descriptorSets;

    uint32_t pushCunstantDataSize;
    
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> xePipeline;
    std::unique_ptr<DescriptorPool> xeDescriptorPool;
    std::unique_ptr<DescriptorSetLayout> xeDescriptorSetLayout;

};

}