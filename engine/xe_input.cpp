#include "xe_input.hpp"

#include <GLFW/glfw3.h>

namespace xe {

static Input* _instance;

Input::Input(Window& window) : window{window} {
  glfwSetKeyCallback(window.getGLFWwindow(), Input::key_callback);
  glfwSetMouseButtonCallback(window.getGLFWwindow(), Input::mouse_callback);
  _instance = this;
}

bool Input::isKeyPressed(int key) {
  return glfwGetKey(window.getGLFWwindow(), key) == GLFW_PRESS;
}

bool Input::wasKeyPressed(int key) {
  if(_pressed[key] == true) {
    _pressed[key] = false;
    return true;
  }
  return false;
}

bool Input::wasKeyReleased(int key) {
  if(_released[key] == true) {
    _released[key] = false;
    return true;
  }
  return false;
}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) _instance->_pressed[key] = true;
  if(action == GLFW_RELEASE) _instance->_released[key] = true;
}

void Input::mouse_callback(GLFWwindow* window, int key, int action, int mods) {
  if(action == GLFW_PRESS) _instance->_pressed[key] = true;
  if(action == GLFW_RELEASE) _instance->_released[key] = true;
}


}