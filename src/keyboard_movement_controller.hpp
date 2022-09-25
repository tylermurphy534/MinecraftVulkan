#pragma once

#include "xe_game_object.hpp"
#include "xe_input.hpp"

#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <limits>

namespace app {

  class KeyboardMovementController {

    public:

      KeyboardMovementController(xe::Input &input, xe::GameObject &viewerObject);
      ~KeyboardMovementController();

      struct KeyMappings {
        int moveLeft = KEY_A;
        int moveRight = KEY_D;
        int moveForward = KEY_W;
        int moveBackward = KEY_S;
        int moveUp = KEY_E;
        int moveDown = KEY_Q;
        int lookLeft = KEY_LEFT;
        int lookRight = KEY_RIGHT;
        int lookUp = KEY_UP;
        int lookDown = KEY_DOWN;
      };

      void update(float dt);

      xe::Input &input;
      xe::GameObject &viewerObject;

      KeyMappings keys{};
      float moveSpeed{3.f};
      float lookSpeed{1.5f};

  };
}