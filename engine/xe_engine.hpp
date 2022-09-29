#pragma once

#include "xe_buffer.hpp"
#include "xe_device.hpp"
#include "xe_renderer.hpp"
#include "xe_camera.hpp"
#include "xe_descriptors.hpp"
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
    
    bool beginFrame() { return xeRenderer.beginFrame(); }
    void endFrame() { xeRenderer.endFrame(); }
    void close() { vkDeviceWaitIdle(xeDevice.device()); }

    bool poll();
    float getFrameTime() { return frameTime; }

    static Engine* getInstance();

  private:
  
    Window xeWindow;
    Device xeDevice;
    Renderer xeRenderer;
    Camera xeCamera;
    Input xeInput;

    std::chrono::_V2::system_clock::time_point currentTime;
    float frameTime;

    float FOV = 50.f;

    friend class RenderSystem;
    friend class Image;
    friend class Model;
};

}