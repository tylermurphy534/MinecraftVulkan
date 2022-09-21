#pragma once

#include "xe_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace xe {

class XeDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(XeDevice &xeDevice) : xeDevice{xeDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        VkSampler *sampler);
    std::unique_ptr<XeDescriptorSetLayout> build() const;

   private:
    XeDevice &xeDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  XeDescriptorSetLayout(
      XeDevice &xeDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~XeDescriptorSetLayout();
  XeDescriptorSetLayout(const XeDescriptorSetLayout &) = delete;
  XeDescriptorSetLayout &operator=(const XeDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  XeDevice &xeDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class XeDescriptorWriter;
};

class XeDescriptorPool {
 public:
  class Builder {
   public:
    Builder(XeDevice &xeDevice) : xeDevice{xeDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<XeDescriptorPool> build() const;

   private:
    XeDevice &xeDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  XeDescriptorPool(
      XeDevice &xeDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~XeDescriptorPool();
  XeDescriptorPool(const XeDescriptorPool &) = delete;
  XeDescriptorPool &operator=(const XeDescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  XeDevice &xeDevice;
  VkDescriptorPool descriptorPool;

  friend class XeDescriptorWriter;
};

class XeDescriptorWriter {
 public:
  XeDescriptorWriter(XeDescriptorSetLayout &setLayout, XeDescriptorPool &pool);

  XeDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  XeDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  XeDescriptorSetLayout &setLayout;
  XeDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

} 

