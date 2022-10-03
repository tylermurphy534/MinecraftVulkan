#include "player_controller.hpp"

namespace app {

PlayerController::PlayerController(xe::Input &input, xe::GameObject &viewerObject)
  : input{input}, viewerObject{viewerObject} {};

PlayerController::~PlayerController() {};

void PlayerController::update(float dt) {
  glm::vec3 rotate{0};
  if(input.isKeyPressed(keys.lookRight)) rotate.y += 1.f;
  if(input.isKeyPressed(keys.lookLeft)) rotate.y -= 1.f;
  if(input.isKeyPressed(keys.lookUp)) rotate.x -= 1.f;
  if(input.isKeyPressed(keys.lookDown)) rotate.x += 1.f;

  if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
    viewerObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
  }

  viewerObject.transform.rotation.x = glm::clamp(viewerObject.transform.rotation.x, -1.5f, 1.5f);
  viewerObject.transform.rotation.y = glm::mod(viewerObject.transform.rotation.y, glm::two_pi<float>());

  float yaw = viewerObject.transform.rotation.y;
  const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
  const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
  const glm::vec3 upDir{0.f, 1.f, 0.f};

  glm::vec3 moveDir{0};
  if(input.isKeyPressed(keys.moveForward)) moveDir += forwardDir;
  if(input.isKeyPressed(keys.moveBackward)) moveDir -= forwardDir;
  if(input.isKeyPressed(keys.moveRight)) moveDir += rightDir;
  if(input.isKeyPressed(keys.moveLeft)) moveDir -= rightDir;
  if(input.isKeyPressed(keys.moveUp)) moveDir += upDir;
  if(input.isKeyPressed(keys.moveDown)) moveDir -= upDir;

  if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
    viewerObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
  }

}

}