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
  if(chunks.count({gridX, gridZ})) {
    return chunks[{gridX, gridZ}];
  } else {
    return nullptr;
  }
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
  blocks[GREEN] = {{getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE), getTexture(GRASS_TOP_TEXTURE)}};
  blocks[STONE] = {{getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE), getTexture(STONE_TEXTURE)}};
  blocks[SNOW] = {{getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE), getTexture(SNOW_TEXTURE)}};
  blocks[SAND] = {{getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE), getTexture(SAND_TEXTURE)}};
  blocks[WATER] = {{getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE), getTexture(WATER_TEXTURE)}};
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
  for(int32_t x=0;x<16;x++) {
    for(int32_t y=0; y<256; y++) {
      for(int32_t z=0; z<16; z++) {
        uint8_t block = c->getBlock(x,y,z);
        if(block == AIR) continue;
        if(c->getBlock(x+1,y,z) == AIR) {
          c->addVerticies(c, 0, x, y, z, block);
        }
        if(c->getBlock(x-1,y,z) == AIR) {
          c->addVerticies(c, 1, x, y, z, block);
        }
        if(c->getBlock(x,y+1,z) == AIR) {
          c->addVerticies(c, 2, x, y, z, block);
        }
        if(c->getBlock(x,y-1,z) == AIR) {
          c->addVerticies(c, 3, x, y, z, block);
        }
        if(c->getBlock(x,y,z+1) == AIR) {
          c->addVerticies(c, 4, x, y, z, block);
        }
        if(c->getBlock(x,y,z-1) == AIR) {
          c->addVerticies(c, 5, x, y, z, block);
        }
      }
    }
  }
  c->reload = true;
  c->working = false;
}

void Chunk::addVerticies(Chunk* c, uint8_t side, int32_t x, int32_t y, int32_t z, uint8_t block) {
  for(int i = 0; i < 6; i ++) {
    c->vertexData.write<float>(px[side * 6 + i][0] + x);
    c->vertexData.write<float>(px[side * 6 + i][1] + y);
    c->vertexData.write<float>(px[side * 6 + i][2] + z);
    c->vertexData.write<float>(nm[side][0]);
    c->vertexData.write<float>(nm[side][1]);
    c->vertexData.write<float>(nm[side][2]);
    c->vertexData.write<float>(uv[i][0]);
    c->vertexData.write<float>(uv[i][1]);
    c->vertexData.write<uint32_t>(static_cast<uint32_t>(blocks[block].textures[side]));
  }
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
      double noise = perlin.octave2D_01((( x + c->gridX * 16) * 0.01), ((z + c->gridZ * 16) * 0.01), 4);
      int height = noise * 40;
      for(int y = 0; y < std::max(height, WATER_LEVEL); y++) {
        int difference = y - WATER_LEVEL;
        if (difference < 0) {
          c->setBlock(x, y, z, WATER);
        } else if(difference < 3) {
          c->setBlock(x, y, z, SAND);
        } else if(difference < 5) {
          c->setBlock(x, y, z, DIRT);
        } else if(difference < 6) {
          c->setBlock(x, y, z, GRASS);
        } else if(difference < 10) {
          c->setBlock(x, y, z, GREEN);
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