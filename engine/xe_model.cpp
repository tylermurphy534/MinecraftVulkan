#include "xe_model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "xe_obj_loader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>
#include <iostream>

namespace xe {

Model::Model(Device &device, const Model::Builder &builder) : xeDevice{device} {
  createVertexBuffers(builder.vertexData, builder.vertexSize);
  createIndexBuffers(builder.indices);
}

Model::~Model() {}

std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filepath) {
  Builder builder{};
  builder.loadModel(filepath);
  return std::make_unique<Model>(device, builder);
}

void Model::createVertexBuffers(const std::vector<float> &vertexData, uint32_t vertexSize) {
  vertexCount = static_cast<uint32_t>(vertexData.size()) / (vertexSize / 4);
  assert(vertexCount >= 3 && "Vertex count must be atleast 3");
  VkDeviceSize bufferSize = vertexData.size() * 4;
 
  Buffer stagingBuffer {
    xeDevice,
    vertexSize,
    vertexCount,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)vertexData.data());

  vertexBuffer = std::make_unique<Buffer>(
    xeDevice,
    vertexSize,
    vertexCount,
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  xeDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t> &indexData) {
  indexCount = static_cast<uint32_t>(indexData.size());
  hasIndexBuffer = indexCount > 0;

  if (!hasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indexData[0]) * indexCount;
  uint32_t indexSize = sizeof(indexData[0]);

  Buffer stagingBuffer {
    xeDevice,
    indexSize,
    indexCount,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)indexData.data());

  indexBuffer = std::make_unique<Buffer>(
    xeDevice,
    indexSize,
    indexCount,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );

  xeDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (hasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void Model::Builder::loadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  vertexData.clear();
  indices.clear();
  vertexSize = 0;

  bool vertex, normal, uvs;

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {

      if(index.vertex_index >= 0) {
        vertexData.push_back(attrib.vertices[3 * index.vertex_index + 0]);
        vertexData.push_back(attrib.vertices[3 * index.vertex_index + 1]);
        vertexData.push_back(attrib.vertices[3 * index.vertex_index + 2]);
        vertex = true;
      }

      if(index.normal_index >= 0) {
        vertexData.push_back(attrib.normals[3 * index.normal_index + 0]);
        vertexData.push_back(attrib.normals[3 * index.normal_index + 1]);
        vertexData.push_back(attrib.normals[3 * index.normal_index + 2]);
        normal = true;
      }

      if(index.texcoord_index >= 0) {
        vertexData.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
        vertexData.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
        uvs = true;
      }

    }
  }

  if(vertex)
    vertexSize += 12;
  if(normal)
    vertexSize += 12;
  if(uvs)
    vertexSize += 8;

}

}