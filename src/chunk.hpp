#pragma once

#include "xe_model.hpp"
#include "PerlinNoise.hpp"

#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <vector>
#include <thread>
#include <memory>
#include <string>
#include <map>

#define INVALID     0
#define AIR         1
#define DIRT        2

namespace app {

class Chunk {

  public:

    static Chunk* newChunk(int32_t gridX, int32_t gridZ, uint32_t world_seed);
    static void reset();

    Chunk(int32_t gridX, int32_t gridZ, uint32_t world_seed);
    ~Chunk() {};

    const int32_t gridX, gridZ;
    const uint32_t world_seed, chunk_seed;

    void createMesh();
    void createMeshAsync();
    std::shared_ptr<xe::Model> getMesh();

    uint8_t getBlock(int32_t x, int32_t y, int32_t z);
    void setBlock(int32_t x, int32_t y, int32_t z, uint8_t block);

    static Chunk* getChunk(int32_t x, int32_t z);

  private:

    void generate();
    void addVerticies(uint8_t side, int32_t x, int32_t y, int32_t z);

    bool reloadRequired{false};
    bool working{false};

    std::shared_ptr<xe::Model> chunkMesh;
    std::vector<float> vertexData;
    std::vector<uint8_t> blocks;
    std::thread worker;

};

const float px[36][3] = {
  // POS X
  {0.5f,0.5f,0.5f},
  {0.5f,-0.5f,0.5f},
  {0.5f,-0.5f,-0.5f},
  {0.5f,-0.5f,-0.5f},
  {0.5f,0.5f,-0.5f},
  {0.5f,0.5f,0.5f},
  // NEG X
  {-0.5f,0.5f,-0.5f},
  {-0.5f,-0.5f,-0.5f},
  {-0.5f,-0.5f,0.5f},
  {-0.5f,-0.5f,0.5f},
  {-0.5f,0.5f,0.5f},
  {-0.5f,0.5f,-0.5f},
  // POS Y
  {0.5f,0.5f,-0.5f},
  {-0.5f,0.5f,-0.5f},
  {-0.5f,0.5f,0.5f},
  {-0.5f,0.5f,0.5f},
  {0.5f,0.5f,0.5f},
  {0.5f,0.5f,-0.5f},
  // NEG Y
  {-0.5f,-0.5f,0.5f},
  {-0.5f,-0.5f,-0.5f},
  {0.5f,-0.5f,-0.5f},
  {0.5f,-0.5f,-0.5f},
  {0.5f,-0.5f,0.5f},
  {-0.5f,-0.5f,0.5f},
  // POS Z
  {-0.5f,0.5f,0.5f},
  {-0.5f,-0.5f,0.5f},
  {0.5f,-0.5f,0.5f},
  {0.5f,-0.5f,0.5f},
  {0.5f,0.5f,0.5f},
  {-0.5f,0.5f,0.5f},
  // NEG Z
  {0.5f,0.5f,-0.5f},
  {0.5f,-0.5f,-0.5f},
  {-0.5f,-0.5f,-0.5f},
  {-0.5f,-0.5f,-0.5f},
  {-0.5f,0.5f,-0.5f},
  {0.5f,0.5f,-0.5f}
};

const float nm[6][3] = {
  {1.f,0.f,0.f},
  {-1.f,0.f,0.f},
  {0.f,1.f,0.f},
  {0.f,-1.f,0.f},
  {0.f,0.f,1.f},
  {0.f,0.f,-1.f}
};


const float uv[6][2] = {
  {1.f,1.f},
  {0.f,1.f},
  {0.f,0.f},
  {0.f,0.f},
  {1.f,0.f},
  {1.f,1.f}
};

}