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

    struct Data {
      std::vector<unsigned char> data{};
      template <typename T>
      void write(T d) {
        unsigned char const * p = reinterpret_cast<unsigned char const *>(&d);
        for(std::size_t i = 0; i < sizeof(T); i++){
          data.push_back(p[i]);
        }
      }
    };

    struct Builder {
      Model::Data vertexData{};
      uint32_t vertexSize;

      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
    };

    Model(Device &device, const Model::Builder &builder);
    ~Model();

    Model(const Model &) = delete;
    Model operator=(const Model &) = delete;

    static Model* createModelFromFile(Device &device, const std::string &filepath);
    
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<unsigned char> &vertexData, uint32_t vertexSize);
    void createIndexBuffers(const std::vector<uint32_t> &indexData);

    Device &xeDevice;

    std::unique_ptr<Buffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;
};

}