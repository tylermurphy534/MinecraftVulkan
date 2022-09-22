#pragma once

#include "xe_window.hpp"
#include "xe_device.hpp"
#include "xe_renderer.hpp"
#include "xe_camera.hpp"
#include "xe_descriptors.hpp"
#include "xe_image.hpp"
#include "xe_sound_device.hpp"

#include <chrono> 
#include <string>
namespace xe {

class XeEngine {

  public:

    XeEngine(int width, int height, std::string name);

    ~XeEngine() {};

    XeEngine(const XeEngine&) = delete;
    XeEngine operator=(const XeEngine&) = delete;

    XeWindow& getWindow() {return xeWindow;}
    XeCamera& getCamera() {return xeCamera;}

    std::shared_ptr<XeModel> loadModelFromFile(const std::string &filename);
    std::shared_ptr<XeModel> loadModelFromData(std::vector<XeModel::Vertex> vertices, std::vector<uint32_t> indices);
    std::shared_ptr<XeImage> loadImage(const std::string &filename);
    
    bool beginFrame() { return xeRenderer.beginFrame(); }
    void endFrame() { xeRenderer.endFrame(); }
    void close() { vkDeviceWaitIdle(xeDevice.device()); }

    bool poll();
    float getFrameTime() { return frameTime; }

  private:

    void loadDescriptorPool();

    XeWindow xeWindow;
    XeDevice xeDevice;
    XeRenderer xeRenderer;
    XeCamera xeCamera;
    XeSoundDevice xeSoundDevice;

    std::chrono::_V2::system_clock::time_point currentTime;
    float frameTime;

    float FOV = 50.f;

    std::unique_ptr<XeDescriptorPool> xeDescriptorPool;

    friend class XeRenderSystem;
};

}