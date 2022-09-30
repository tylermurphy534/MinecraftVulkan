#include "chunk.hpp"

namespace app {

//
//  CHUNK CONSTRUCTORS AND DECONSTUCTORS
//

Chunk::Chunk(int32_t gridX, int32_t gridZ, uint32_t world_seed) 
  : world_seed{world_seed},
    chunk_seed{(world_seed * gridX) + (world_seed * gridZ) / 2},
    gridX{gridX},
    gridZ{gridZ} {
  chunkMesh = nullptr;
}

Chunk::~Chunk() {
  if(worker.joinable())
    worker.join();
  xe::Model::deleteModel(chunkMesh);
  vertexData.data.clear();
  cubes.clear();
}

//
//  CHUNK CREATION, DELETION, AND RETREVAL
//

static std::map<std::pair<int32_t, int32_t>, Chunk*> chunks{};

Chunk* Chunk::newChunk(int32_t gridX, int32_t gridZ, uint32_t world_seed) {
  Chunk* chunk = new Chunk(gridX, gridZ, world_seed);
  chunks[{gridX, gridZ}] = std::move(chunk);
  return chunks[{gridX, gridZ}];
}

Chunk* Chunk::getChunk(int32_t gridX, int32_t gridZ) {
  auto it = chunks.find({gridX, gridZ});
  if(it != chunks.end()) {
    return it->second;
  }
  return nullptr;
}

void Chunk::deleteChunk(int32_t gridX, int32_t gridZ) {
  Chunk* chunk = getChunk(gridX, gridZ);
  if(chunk == nullptr) return; // Chunk does not exist or is already deleted
  delete chunk;
  chunks.erase({gridX, gridZ});
}

//
//  CHUNK TEXTURE AND BLOCK LOADING
//

static std::map<uint8_t, Block> blocks{};
static std::map<std::string, uint32_t> texturesIds{};
static std::vector<xe::Image*> textures{};

uint32_t getTexture(const std::string& filePath) {
  if(!texturesIds.count(filePath)) {
    xe::Image* image = xe::Image::createImage(filePath, false);
    texturesIds[filePath] = static_cast<uint32_t>(textures.size());
    textures.push_back(image);
  }
  return texturesIds[filePath];
}

std::vector<xe::Image*>& Chunk::getTextures() {
  return textures;
}

void Chunk::load() {
  blocks[DIRT] = {{getTexture(DIRT_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(DIRT_TEXTURE)}};
  blocks[GRASS] = {{getTexture(GRASS_TEXTURE), getTexture(GRASS_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(GRASS_TEXTURE), getTexture(GRASS_TEXTURE)}};
  blocks[FULL_GRASS] = {{getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE)}};
  blocks[STONE] = {{getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE)}};
  blocks[SNOW] = {{getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE)}};
  blocks[SAND] = {{getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE)}};
  blocks[WATER] = {{getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE)}};
  blocks[SHRUB] = {{getTexture(SHRUB_TEXTURE), getTexture(SHRUB_TEXTURE), getTexture(SHRUB_TOP_TEXTURE), getTexture(DIRT_TEXTURE), getTexture(SHRUB_TEXTURE), getTexture(SHRUB_TEXTURE)}};
  blocks[FULL_SHRUB] = {{getTexture(SHRUB_TOP_TEXTURE), getTexture(SHRUB_TOP_TEXTURE), getTexture(SHRUB_TOP_TEXTURE), getTexture(SHRUB_TOP_TEXTURE), getTexture(SHRUB_TOP_TEXTURE), getTexture(SHRUB_TOP_TEXTURE)}};
}

void Chunk::unload() {
  for(const auto &image: textures) {
    xe::Image::deleteImage(image);
  }
  for(const auto &[key, chunk]: chunks) {
    delete chunk;
  }
  chunks.clear();
  textures.clear();
}

//
//  CHUNK MESH CREATION FOR BOTH SYNC AND ASYNC
//

void Chunk::createMeshAsync(Chunk* c) {
  if(c == nullptr) return;
  if(c->working) return;
  c->working = true;
  if(c->worker.joinable())
    c->worker.join();
  c->worker = std::thread(createMesh, c);
}

struct FMask {
  int block;
  int normal;
};

bool CompareMask(FMask a, FMask b){
  if(a.block == INVALID || b.block == INVALID) return false;
  return a.block == b.block && a.normal == b.normal;
}

void AddVertex(xe::Model::Data& data, glm::vec3 Pos, glm::vec3 Nor, FMask Mask, glm::vec3 AxisMask, float Uv[2]) {
  data.write<float>(Pos[0]);
  data.write<float>(Pos[1]);
  data.write<float>(Pos[2]);
  data.write<float>(Nor[0]);
  data.write<float>(Nor[1]);
  data.write<float>(Nor[2]);
  data.write<float>(Uv[0]);
  data.write<float>(Uv[1]);

  int i = AxisMask[1]*2+AxisMask[2]*4+(Mask.normal<0?1:0);
  data.write<uint32_t>(blocks[Mask.block].textures[i]);
}

void CreateQuad(xe::Model::Data& data, FMask Mask, glm::vec3 AxisMask, glm::vec3 V1, glm::vec3 V2, glm::vec3 V3, glm::vec3 V4, uint32_t width, uint32_t height) {
  const auto Normal = glm::vec3(AxisMask) * glm::vec3(Mask.normal);
  std::vector<glm::vec3> verticies = {V1, V2, V3, V4};

  float uv[4][2];
  
  if (AxisMask[0] == 1) {
    uv[0][0] = width; uv[0][1] = height;
    uv[1][0] = 0; uv[1][1] = height;
    uv[2][0] = width; uv[2][1] = 0;
    uv[3][0] = 0; uv[3][1] = 0;
  } else {
    uv[0][0] = height; uv[0][1] = width;
    uv[1][0] = height; uv[1][1] = 0;
    uv[2][0] = 0; uv[2][1] = width;
    uv[3][0] = 0; uv[3][1] = 0;
  }

  AddVertex(data, verticies[0], Normal, Mask, AxisMask, uv[0]);
  AddVertex(data, verticies[2 + Mask.normal], Normal, Mask, AxisMask, uv[3]);
  AddVertex(data, verticies[2 - Mask.normal], Normal, Mask, AxisMask, uv[2]);
  AddVertex(data, verticies[3], Normal, Mask, AxisMask, uv[3]);
  AddVertex(data, verticies[1 - Mask.normal], Normal, Mask, AxisMask, uv[0]);
  AddVertex(data, verticies[1 + Mask.normal], Normal, Mask, AxisMask, uv[1]);

}

void Chunk::createMesh(Chunk* c) {
  if(c == nullptr) return;
  if(!isGenerated(c->gridX-1, c->gridZ) ||
     !isGenerated(c->gridX+1, c->gridZ) ||
     !isGenerated(c->gridX, c->gridZ-1) ||
     !isGenerated(c->gridX, c->gridZ+1)) {
    c->working = false;
    return;
  }
  c->vertexData.data.clear();
  for (int Axis = 0; Axis < 3; ++Axis) {
    const int Axis1 = (Axis + 1) % 3;
    const int Axis2 = (Axis + 2) % 3;

    const int MainAxisLimit = Axis == 1 ? 256 : 16; 
    int Axis1Limit = Axis1 == 1 ? 256 : 16;
    int Axis2Limit = Axis2 == 1 ? 256 : 16;

    auto DeltaAxis1 = glm::vec3(0.f);
    auto DeltaAxis2 = glm::vec3(0.f);

    auto ChunkItr = glm::vec3(0.f);
    auto AxisMask = glm::vec3(0.f);

    AxisMask[Axis] = 1;

    std::vector<FMask> Mask;
    Mask.resize(Axis1Limit * Axis2Limit);

    for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;) {
      int N = 0;
      
      for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2]) {
        for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1]) {
          const auto CurrentBlock = c->getBlock(ChunkItr[0], ChunkItr[1], ChunkItr[2]);
          const auto CompareBlock = c->getBlock(ChunkItr[0] + AxisMask[0], ChunkItr[1] + AxisMask[1] , ChunkItr[2] + AxisMask[2]);

          const bool CurrentBlockOpaque = CurrentBlock != AIR;
          const bool CompareBlockOpaque = CompareBlock != AIR;

          if (CurrentBlockOpaque == CompareBlockOpaque) {
            Mask[N++] = FMask { INVALID, 0 };
          } else  if (CurrentBlockOpaque) {
            Mask[N++] = FMask { CurrentBlock, 1};
          } else {
            Mask[N++] = FMask { CompareBlock, -1};
          }
        }
      }

      ++ChunkItr[Axis];
      N = 0;

      for (int j = 0; j < Axis2Limit; ++j) {
        for (int i = 0; i < Axis1Limit;) {
          if(Mask[N].normal != 0) {
            const auto CurrentMask = Mask[N];
            ChunkItr[Axis1] = i;
            ChunkItr[Axis2] = j;

            int width;

            for(width = 1; i + width < Axis1Limit && CompareMask(Mask[N + width], CurrentMask); ++width) {}

            int height;
            bool done = false;

            for (height = 1; j + height < Axis2Limit; ++height) {
              for(int k = 0; k < width; ++k) {
                if(CompareMask(Mask[N + k + height * Axis1Limit], CurrentMask)) continue;

                done = true;
                break;
              }

              if(done) break;
            }

            DeltaAxis1[Axis1] = width;
            DeltaAxis2[Axis2] = height;

            CreateQuad(c->vertexData, CurrentMask, AxisMask, 
              ChunkItr,
              ChunkItr + DeltaAxis1,
              ChunkItr + DeltaAxis2,
              ChunkItr + DeltaAxis1 + DeltaAxis2,
              height,
              width
            );

            DeltaAxis1 = glm::vec3(0.f);
            DeltaAxis2 = glm::vec3(0.f);

            for (int l = 0; l < height; ++l) {
              for(int k = 0; k < width; ++k) {
                Mask[N + k + l * Axis1Limit] = FMask { INVALID, 0 };
              }
            }

            i += width;
            N += width;

          } else {

            i++;
            N++;

          }

        }
      }
    }

  }
  c->reload = true;
  c->working = false;
}

//
//  CHUNK GENERATION FOR BOTH SYNC AND ASYNC
//

void Chunk::generateAsync(Chunk* c) {
  if(c == nullptr) return;
  if(c->working) return;
  c->working = true;
  if(c->worker.joinable())
    c->worker.join();
  c->worker = std::thread(generate, c);
}

void Chunk::generate(Chunk* c) {
  c->cubes.resize(16*16*256);
  
  const PerlinNoise perlin{c->world_seed};

  for(int x = 0; x < 16; x++) {
    for(int z = 0; z < 16; z++) {
      double biome = perlin.octave2D_01((( x + c->gridX * 13) * 0.0005), ((z + c->gridZ * 13) * 0.0005), 4) * 2;
      double continent = perlin.octave2D_01((( x + c->gridX * 16) * 0.001), ((z + c->gridZ * 16) * 0.001), 4) * 10 - 5;
      double noise = perlin.octave2D_01((( x + c->gridX * 16) * 0.01), ((z + c->gridZ * 16) * 0.01), 4);
      int height = noise * 40 + continent;
      for(int y = 0; y < std::max(height, WATER_LEVEL); y++) {
        int difference = y - WATER_LEVEL;
        if (difference < 0) {
          c->setBlock(x, y, z, WATER);
        } else if(difference < 3) {
          c->setBlock(x, y, z, SAND);
        } else if(difference < 5) {
          c->setBlock(x, y, z, DIRT);
        } else if(difference < 6) {
          c->setBlock(x, y, z, biome > 1 ? GRASS : SHRUB);
        } else if(difference < 10) {
          c->setBlock(x, y, z, biome > 1 ? FULL_GRASS : FULL_SHRUB);
        } else if(difference < 16) {
          c->setBlock(x, y, z, STONE);
        } else if(difference < 18) {
          c->setBlock(x, y, z, SNOW);
        }
      }
    }
  }

  c->generated = true;
  c->working = false;
}

//
//  CHUNK GETTERS AND SETTORS
//

xe::Model* Chunk::getMesh() {
  if(reload) {
    if(chunkMesh != nullptr) {
      xe::Model::deleteModel(chunkMesh);
      chunkMesh = nullptr;
    }
    if(worker.joinable())
      worker.join();
    xe::Model::Builder builder{};
    builder.vertexData = vertexData;
    builder.vertexSize = 36;
    chunkMesh = xe::Model::createModel(builder);
    vertexData.data.clear();
    reload = false;
  }
  return chunkMesh;
}

uint8_t Chunk::getBlock(int32_t x, int32_t y, int32_t z) {
  if(y > 256) return AIR;
  if(y < 0) return INVALID;
  int chunkX = gridX;
  int chunkZ = gridZ;
  if(x < 0) {
    chunkX--;
  } else if(x > 15) {
    chunkX++;
  }
  if(z < 0) {
    chunkZ--;
  } else if(z > 15) {
    chunkZ++;
  }
  x = (x+16)%16;
  z = (z+16)%16;
  if(chunkX == gridX && chunkZ == gridZ) {
    int index = x + (z * 16) + (y * 256);
    return cubes[index];
  } else {
    Chunk* chunk = getChunk(chunkX, chunkZ);
    if(chunk == NULL) {
      return INVALID;
    } else {
      int index = x + (z * 16) + (y * 256);
      return chunk->cubes[index];
    }
  }
}

void Chunk::setBlock(int32_t x, int32_t y, int32_t z, uint8_t block) {
  int index = x + (z * 16) + (y * 256);
  cubes[index] = block;
}

bool Chunk::isGenerated(int32_t gridX, int32_t gridZ) {
  Chunk* chunk = Chunk::getChunk(gridX, gridZ);
  if(chunk == nullptr) return false;
  return chunk->generated;
}

}