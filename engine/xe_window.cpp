#include "xe_window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

#include <stdexcept>

namespace xe {

  XeWindow::XeWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
  }

  XeWindow::~XeWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void XeWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  void XeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface");
    }
  }

  void XeWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height){
    auto xeWindow = reinterpret_cast<XeWindow *>(glfwGetWindowUserPointer(window));
    xeWindow->frameBufferResized = true;
    xeWindow->width = width;
    xeWindow->height = height;
  }

}