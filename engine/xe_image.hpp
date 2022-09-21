#pragma once

#include "xe_device.hpp"

#include <string>

namespace xe {
  
class XeImage {

  public:
  
    XeImage(XeDevice &xeDevice, const std::string &filename);
    ~XeImage();

    XeImage(const XeImage&) = delete;
    XeImage operator=(const XeImage&) = delete;

  private:

    void createTextureImage(const std::string &filename);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    XeDevice &xeDevice;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    friend class XeRenderSystem;

};

}
