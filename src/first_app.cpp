#include "first_app.hpp"

#include "xe_camera.hpp"
#include "xe_game_object.hpp"
#include "xe_model.hpp"
#include "keyboard_movement_controller.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono> 
#include <cassert>
#include <stdexcept>
#include <iostream>
namespace xe {

struct UniformBuffer {
  alignas(16) glm::mat4 projectionView{1.f};
  alignas(4) glm::vec3 lightDirection = glm::normalize(glm::vec3{-1.f, 3.f, 1.f});
};

struct PushConstant {
  alignas(16) glm::mat4 modelMatrix{1.f};
  alignas(16) glm::mat4 normalMatrix{1.f};
};

FirstApp::FirstApp() : xeEngine{WIDTH, HEIGHT, "Hello, Vulkan!"} {
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::unique_ptr<XeRenderSystem> simpleRenderSystem = xeEngine.createRenderSystem(
    "res/shaders/simple_shader.vert.spv",
    "res/shaders/simple_shader.frag.spv", 
    sizeof(PushConstant), 
    sizeof(UniformBuffer));

  XeCamera camera{};
  camera.setViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = XeGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!xeEngine.getWindow().shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(xeEngine.getWindow().getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = xeEngine.getRenderer().getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

    if(xeEngine.beginFrame()) {

      PushConstant pc{};
      pc.modelMatrix = gameObjects[0].transform.mat4();
      pc.normalMatrix = gameObjects[0].transform.normalMatrix();

      UniformBuffer ubo{};
      ubo.projectionView = camera.getProjection() * camera.getView();
      
      xeEngine.render(
        *simpleRenderSystem,
        gameObjects,
        &pc,
        sizeof(pc),
        &ubo,
        sizeof(ubo)
      );

      xeEngine.endFrame();

    }
  }

  xeEngine.close();

}

void FirstApp::loadGameObjects() {
  std::shared_ptr<XeModel> xeModel = xeEngine.createModel("res/models/stanford-dragon.obj");

  auto cube = XeGameObject::createGameObject();
  cube.model = xeModel;
  cube.transform.translation = {.0f, .0f, 2.5f};
  cube.transform.scale = {.5f, .5f, .5f};
  gameObjects.push_back(std::move(cube));
}

}