#include "first_app.hpp"

#include "xe_camera.hpp"
#include "xe_game_object.hpp"
#include "xe_model.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono> 
#include <cassert>
#include <stdexcept>

namespace xe {

struct GlobalUbo {
  glm::mat4 projectionView{1.f};
  glm::vec3 lightDirection = glm::normalize(glm::vec3{-1.f, 3.f, 1.f});
};

FirstApp::FirstApp() {
  globalPool = XeDescriptorPool::Builder(xeDevice)
    .setMaxSets(XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, XeSwapChain::MAX_FRAMES_IN_FLIGHT)
    .build();
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {

  std::vector<std::unique_ptr<XeBuffer>> uboBuffers(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<XeBuffer>(
      xeDevice,
      sizeof(GlobalUbo),
      XeSwapChain::MAX_FRAMES_IN_FLIGHT,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  auto globalSetLayout = XeDescriptorSetLayout::Builder(xeDevice)
    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(XeSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    XeDescriptorWriter(*globalSetLayout, *globalPool)
      .writeBuffer(0, &bufferInfo)
      .build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem{xeDevice, xeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
  XeCamera camera{};
  camera.setViewTarget(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = XeGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!xeWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(xeWindow.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = xeRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

    if(auto commandBuffer = xeRenderer.beginFrame()) {

      int frameIndex = xeRenderer.getFrameIndex();
      XeFrameInfo frameInfo{
        frameIndex,
        frameTime,
        commandBuffer,
        camera,
        globalDescriptorSets[frameIndex]
      };
      
      // update
      GlobalUbo ubo{};
      ubo.projectionView = camera.getProjection() * camera.getView();
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      xeRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
      xeRenderer.endSwapChainRenderPass(commandBuffer);
      xeRenderer.endFrame();
    }

  }

  vkDeviceWaitIdle(xeDevice.device());

}

void FirstApp::loadGameObjects() {
  std::shared_ptr<XeModel> xeModel = XeModel::createModelFromFile(xeDevice, "res/models/stanford-dragon.obj");

  auto cube = XeGameObject::createGameObject();
  cube.model = xeModel;
  cube.transform.translation = {.0f, .0f, 2.5f};
  cube.transform.scale = {.5f, .5f, .5f};
  gameObjects.push_back(std::move(cube));
}

}