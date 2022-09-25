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
#include <map>

namespace xe {

class RenderSystem {
  public:
  
    class Builder {
      public:
        Builder(Engine &xeEngine, std::string vert, std::string frag) : xeEngine{xeEngine}, vert{vert}, frag{frag} {}

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

        Builder& setCulling(bool enabled) {
          cullingEnabled = enabled;
          return *this;
        }

        std::unique_ptr<RenderSystem> build() {
          return std::make_unique<RenderSystem>(xeEngine, std::move(vert), std::move(frag), std::move(uniformBindings), std::move(imageBindings), std::move(pushCunstantDataSize), std::move(cullingEnabled));
        }

      private:

        std::map<uint32_t, uint32_t> uniformBindings{};
        std::map<uint32_t, Image*> imageBindings{};
        uint32_t pushCunstantDataSize{0};

        std::string vert;
        std::string frag;

        bool cullingEnabled{false};

        Engine &xeEngine;
    };

    RenderSystem(
      Engine &xeEngine,
      std::string vert,
      std::string frag,
      std::map<uint32_t, uint32_t> uniformBindings,
      std::map<uint32_t, Image*> imageBindings,
      uint32_t pushCunstantDataSize,
      bool cullingEnabled
    );

    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem operator=(const RenderSystem &) = delete;

    void start();
    void loadPushConstant(void *pushConstantData);
    void loadUniformObject(uint32_t binding, void *uniformBufferData);
    void loadTexture(uint32_t binding, Image *image);
    void render(GameObject &gameObject);
    void stop();

  private:
  
    void createTextureSampler();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorSets();
    void updateDescriptorSet(int frameIndex, bool allocate);
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass, std::string vert, std::string frag, bool cullingEnabled);

    bool boundPipeline{false};
    bool boundDescriptor{false};

  
    Device& xeDevice;
    Renderer& xeRenderer;

    std::map<uint32_t, std::vector<std::unique_ptr<Buffer>>> uboBuffers{};
    std::map<uint32_t, uint32_t> uniformBindings;
    std::map<uint32_t, Image*> imageBindings;
    std::vector<VkDescriptorSet> descriptorSets;

    uint32_t pushCunstantDataSize;

    VkSampler textureSampler;
    
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Pipeline> xePipeline;
    std::unique_ptr<DescriptorPool> &xeDescriptorPool;
    std::unique_ptr<DescriptorSetLayout> xeDescriptorSetLayout;

};

}