#pragma once

#include "xe_device.hpp"
#include "xe_buffer.hpp"
#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace xe {

class Model {
  public:

    struct Builder {
      std::vector<float> vertexData{};
      uint32_t vertexSize;

      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
    };

    Model(Device &device, const Model::Builder &builder);
    ~Model();

    Model(const Model &) = delete;
    Model operator=(const Model &) = delete;

    static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);
    
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<float> &vertexData, uint32_t vertexSize);
    void createIndexBuffers(const std::vector<uint32_t> &indexData);

    Device &xeDevice;

    std::unique_ptr<Buffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;
};

}