#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace xe {
    
class XeWindow {
  public:
    XeWindow(int w, int h, std::string name);
    ~XeWindow();

    XeWindow(const XeWindow &) = delete;
    XeWindow &operator=(const XeWindow &);

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    bool wasWindowResized() { return frameBufferResized; }
    void resetWindowResizedFlag() { frameBufferResized = false; }
    GLFWwindow *getGLFWwindow() const { return window; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    void initWindow();  
    
    int width;
    int height;
    bool frameBufferResized = false;
    
    std::string windowName;
    GLFWwindow *window;
};
 
}