#pragma once

#include "xe_device.hpp"
#include "xe_renderer.hpp"
#include "xe_camera.hpp"
#include "xe_descriptors.hpp"
#include "xe_image.hpp"
#include "xe_input.hpp"
#include "xe_sound.hpp"

#include <chrono> 
#include <string>
#include <iostream>
#include <AL/alc.h> 
#include <AL/alut.h>

namespace xe {

class Engine {

  public:

    Engine(int width, int height, std::string name, const char *icon);

    ~Engine();

    Engine(const Engine&) = delete;
    Engine operator=(const Engine&) = delete;

    Input& getInput() {return xeInput;}
    Camera& getCamera() {return xeCamera;}
    Device& getDevice() {return xeDevice;}

    std::shared_ptr<Model> loadModelFromFile(const std::string &filename);
    std::shared_ptr<Model> loadModelFromData(std::vector<unsigned char> vertexData, uint32_t vertexSize, std::vector<uint32_t> indices);
    Image* loadImageFromFile(const std::string &filename, bool anisotropic = true);
    
    bool beginFrame() { return xeRenderer.beginFrame(); }
    void endFrame() { xeRenderer.endFrame(); }
    void close() { vkDeviceWaitIdle(xeDevice.device()); }

    bool poll();
    float getFrameTime() { return frameTime; }

    static Engine* getInstance();

  private:

    void loadDescriptorPool();

    Window xeWindow;
    Device xeDevice;
    Renderer xeRenderer;
    Camera xeCamera;
    Input xeInput;

    std::chrono::_V2::system_clock::time_point currentTime;
    float frameTime;

    float FOV = 50.f;

    std::unique_ptr<DescriptorPool> xeDescriptorPool;

    friend class RenderSystem;
};

}