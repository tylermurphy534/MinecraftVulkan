#pragma once

#include "xe_camera.hpp"

#include <vulkan/vulkan.h>

namespace xe {

struct XeFrameInfo {
    int frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    XeCamera &camera;
};

}