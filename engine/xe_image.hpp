#pragma once

#include "xe_device.hpp"

#include <string>

namespace xe {
  
class Image {

  public:
  
    Image(Device &xeDevice, const std::string &filename);
    ~Image();

    Image(const Image&) = delete;
    Image operator=(const Image&) = delete;

  private:

    void createTextureImage(const std::string &filename);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createTextureImageView();
    void createTextureSampler(); 

    Device &xeDevice;

    uint32_t mipLevels;
    VkImage textureImage;
    VkSampler textureSampler;
    VkImageView textureImageView;
    VkDeviceMemory textureImageMemory;

    friend class RenderSystem;

};

}
