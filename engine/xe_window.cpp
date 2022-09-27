#include "xe_window.hpp"
#include "stb_image.h"

namespace xe {

  Window::Window(int w, int h, std::string name, const char *icon) : width{w}, height{h}, windowName{name} {
    initWindow();
    setIcon(icon);
  }

  Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  void Window::setIcon(const char *icon) {
    if(icon == NULL) return;
    GLFWimage images[1]; 
    images[0].pixels = stbi_load(icon, &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images); 
    stbi_image_free(images[0].pixels);
  }

  void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface");
    }
  }

  void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height){
    auto xeWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    xeWindow->frameBufferResized = true;
    xeWindow->width = width;
    xeWindow->height = height;
  }

}