#include "xe_input.hpp"

#include <GLFW/glfw3.h>

namespace xe {

static XeInput* _instance;

XeInput::XeInput(XeWindow& window) : window{window} {
  glfwSetKeyCallback(window.getGLFWwindow(), XeInput::key_callback);
  glfwSetMouseButtonCallback(window.getGLFWwindow(), XeInput::mouse_callback);
  _instance = this;
}

bool XeInput::isKeyPressed(int key) {
  return glfwGetKey(window.getGLFWwindow(), key) == GLFW_PRESS;
}

bool XeInput::wasKeyPressed(int key) {
  if(_pressed[key] == true) {
    _pressed[key] = false;
    return true;
  }
  return false;
}

bool XeInput::wasKeyReleased(int key) {
  if(_released[key] == true) {
    _released[key] = false;
    return true;
  }
  return false;
}

void XeInput::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) _instance->_pressed[key] = true;
  if(action == GLFW_RELEASE) _instance->_released[key] = true;
}

void XeInput::mouse_callback(GLFWwindow* window, int key, int action, int mods) {
  if(action == GLFW_PRESS) _instance->_pressed[key] = true;
  if(action == GLFW_RELEASE) _instance->_released[key] = true;
}


}