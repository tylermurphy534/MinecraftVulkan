#include "xe_buffer.hpp"

#include <cassert>
#include <cstring>

namespace xe {

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

Buffer::Buffer(
    Device &device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : xeDevice{device},
      instanceSize{instanceSize},
      instanceCount{instanceCount},
      usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
  alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  bufferSize = alignmentSize * instanceCount;
  device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

Buffer::~Buffer() {
  unmap();
  vkDestroyBuffer(xeDevice.device(), buffer, nullptr);
  vkFreeMemory(xeDevice.device(), memory, nullptr);
}

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
  assert(buffer && memory && "Called map on buffer before create");
  return vkMapMemory(xeDevice.device(), memory, offset, size, 0, &mapped);
}

void Buffer::unmap() {
  if (mapped) {
    vkUnmapMemory(xeDevice.device(), memory);
    mapped = nullptr;
  }
}

void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
  assert(mapped && "Cannot copy to unmapped buffer");

  if (size == VK_WHOLE_SIZE) {
    memcpy(mapped, data, bufferSize);
  } else {
    char *memOffset = (char *)mapped;
    memOffset += offset;
    memcpy(memOffset, data, size);
  }
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkFlushMappedMemoryRanges(xeDevice.device(), 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mappedRange = {};
  mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return vkInvalidateMappedMemoryRanges(xeDevice.device(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
  return VkDescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}

void Buffer::writeToIndex(void *data, int index) {
  writeToBuffer(data, instanceSize, index * alignmentSize);
}

VkResult Buffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
  return descriptorInfo(alignmentSize, index * alignmentSize);
}

VkResult Buffer::invalidateIndex(int index) {
  return invalidate(alignmentSize, index * alignmentSize);
}

}

