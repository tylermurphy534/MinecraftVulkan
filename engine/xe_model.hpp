#pragma once

#include "xe_device.hpp"
#include "xe_buffer.hpp"
#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace xe {

class XeModel {
  public:

    struct Vertex {
      glm::vec3 position;
      glm::vec3 color;
      glm::vec3 normal;
      glm::vec2 uv;
      
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex &other) const {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
      }
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void loadModel(const std::string &filepath);
    };

    XeModel(XeDevice &device, const XeModel::Builder &builder);
    ~XeModel();

    XeModel(const XeModel &) = delete;
    XeModel operator=(const XeModel &) = delete;

    static std::unique_ptr<XeModel> createModelFromFile(XeDevice &device, const std::string &filepath);
    
    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    XeDevice &xeDevice;

    std::unique_ptr<XeBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<XeBuffer> indexBuffer;
    uint32_t indexCount;
};

}