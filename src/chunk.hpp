#pragma once

#include "xe_model.hpp"
#include "xe_engine.hpp"
#include "xe_image.hpp"

#include "PerlinNoise.hpp"

#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <vector>
#include <thread>
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#define INVALID             -1
#define AIR                 0
#define DIRT                1
#define GRASS               2
#define FULL_GRASS          3
#define STONE               4
#define SNOW                5
#define SAND                6
#define WATER               7
#define SHRUB               8
#define FULL_SHRUB          9

#define DIRT_TEXTURE        "res/image/dirt.png"
#define GRASS_TEXTURE       "res/image/grass.png"
#define GRASS_TOP_TEXTURE   "res/image/grass_top.png"
#define STONE_TEXTURE       "res/image/stone.png"
#define SNOW_TEXTURE        "res/image/snow.png"
#define SAND_TEXTURE        "res/image/sand.png"
#define WATER_TEXTURE       "res/image/water.png"
#define SHRUB_TEXTURE       "res/image/shrub.png"
#define SHRUB_TOP_TEXTURE   "res/image/shrub_top.png"

static constexpr int WATER_LEVEL = 20;

namespace app {

struct Block {
  uint32_t textures[6];
};

class Chunk {

  public:

    static void load();
    static void unload();
    static std::vector<xe::Image*>& getTextures();

    static Chunk* newChunk(int32_t gridX, int32_t gridZ, uint32_t world_seed);
    static Chunk* getChunk(int32_t gridX, int32_t gridZ);
    static void deleteChunk(int32_t gridX, int32_t gridZ);

    static void createMesh(Chunk* c);
    static void createMeshAsync(Chunk* c);

    static void generate(Chunk* c);
    static void generateAsync(Chunk* c);

    xe::Model* getMesh();
    uint8_t getBlock(int32_t x, int32_t y, int32_t z);
    void setBlock(int32_t x, int32_t y, int32_t z, uint8_t block);

    static bool isGenerated(int32_t gridX, int32_t gridZ);

    const int32_t gridX, gridZ;
    const uint32_t world_seed, chunk_seed;

  private:

    Chunk(int32_t gridX, int32_t gridZ, uint32_t world_seed);
    ~Chunk();

    static void addVerticies(Chunk* c, uint8_t side, int32_t x, int32_t y, int32_t z, uint8_t block);

    bool generated{false};
    bool reload{false};
    bool working{false};

    xe::Model* chunkMesh;
    xe::Model::Data vertexData;
    std::vector<uint8_t> cubes;
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
  {1.f,0.f},
  {1.f,1.f},
  {0.f,1.f},
  {0.f,1.f},
  {0.f,0.f},
  {1.f,0.f}
};

}