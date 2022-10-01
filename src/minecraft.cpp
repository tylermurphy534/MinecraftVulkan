#include "minecraft.hpp"

#include <chrono>
using namespace std::chrono;

namespace app {

Minecraft::Minecraft() : engine{WIDTH, HEIGHT, "Minecraft Vulkan", "res/image/icon.png"} {};

Minecraft::~Minecraft() {}

void Minecraft::run() {

  Chunk::load();

  auto viewer = xe::GameObject::createGameObject();
  viewer.transform.translation = {0.f, 40.f, 0.f};
  viewer.transform.rotation.y = glm::radians(45.f);

  World world {viewer, 10, 12345};

  xe::Sound sound{"res/sound/when_the_world_ends.wav"};
  sound.setLooping(true);
  sound.play();
    
  PlayerController playerController{engine.getInput(), viewer};

  while (engine.poll()) {

    playerController.update(engine.getFrameTime());

    world.reloadChunks();

    if(engine.beginFrame()) {
      world.render(engine.getCamera());
      engine.endFrame();
    }

  }

  engine.close();

  Chunk::unload();

}

}