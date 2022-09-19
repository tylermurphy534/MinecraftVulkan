#pragma once

#include "xe_model.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/fwd.hpp>
#include <memory>

namespace xe {

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.f, 1.f, 1.f};
  glm::vec3 rotation{};


  glm::mat4 mat4();
  glm::mat3 normalMatrix();

};

class XeGameObject {
  public:
    using id_t = unsigned int;

    static XeGameObject createGameObject() {
      static id_t currentId = 0;
      return XeGameObject(currentId++);
    }

    XeGameObject(const XeGameObject &) = delete;
    XeGameObject &operator=(const XeGameObject &) = delete;
    XeGameObject(XeGameObject&&) = default;
    XeGameObject &operator=(XeGameObject &&) = default;

    id_t getId() { return id; }

    std::shared_ptr<XeModel> model{};
    glm::vec3 color{};
    TransformComponent transform;

  private:
    XeGameObject(id_t objId) : id{objId} {}

    id_t id;
};

}