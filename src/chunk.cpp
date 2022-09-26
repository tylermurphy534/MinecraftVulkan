#include "chunk.hpp"
#include "xe_engine.hpp"

namespace app {

static std::map<std::string, std::unique_ptr<Chunk>> chunks{};

Chunk::Chunk(uint32_t gridX, uint32_t gridZ, uint32_t world_seed) 
  : world_seed{world_seed},
    chunk_seed{(world_seed * gridX) + (world_seed * gridZ) / 2},
    gridX{gridX},
    gridZ{gridZ} {
  generate();
}

Chunk* Chunk::newChunk(uint32_t gridX, uint32_t gridZ, uint32_t world_seed) {
  std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(gridX, gridZ, world_seed);
  std::string key = gridX + "." + gridZ;
  chunks[key] = std::move(chunk);
  return chunks[key].get();
}

void Chunk::reset() {
  chunks.clear();
}

std::shared_ptr<xe::Model> Chunk::getMesh() {
  if(reloadRequired) {
    delete chunkMesh.get();
    xe::Model::Builder builder{};
    builder.vertexData = vertexData;
    builder.vertexSize = 32;
    chunkMesh = std::make_shared<xe::Model>(xe::Engine::getInstance()->getDevice(), builder);
  }
  return chunkMesh;
}

uint8_t Chunk::getBlock(uint32_t x, uint32_t y, uint32_t z) {
  if(y < 0 || y > 256) return AIR;
  int chunkX = gridX;
  int chunkZ = gridZ;
  if(x < 0) {
    chunkX--;
    x = 15;
  } else if(x > 16) {
    chunkX ++;
    x = 0;
  }
  if(z < 0) {
    chunkZ--;
    z = 15;
  } else if(z > 16) {
    chunkZ ++;
    z = 0;
  }
  if(chunkX == gridX && chunkZ == gridZ) {
    int index = (z * 16 * 256) + (y * 16) + x;
    return blocks[index];
  } else {
    Chunk* chunk = getChunk(chunkX, chunkZ);
    if(chunk == nullptr) {
      return AIR;
    } else {
      int index = (z * 16 * 256) + (y * 16) + x;
      return chunk->blocks[index];
    }
  }
}

void Chunk::setBlock(uint32_t x, uint32_t y, uint32_t z, uint8_t block) {
  int index = (z * 16 * 256) + (y * 16) + x;
  blocks[index] = block;
}

Chunk* Chunk::getChunk(uint32_t x, uint32_t z) {
  std::string key = x + "." + z;
  if(chunks.count(key))
    return chunks[key].get();
  else
    return nullptr;
}

void Chunk::createMeshAsync() {
  if(working) return;
  // worker = std::thread(createMesh);
}

void Chunk::createMesh() {
  working = true;
  vertexData.clear();
  for(int x=0;x<16;x++) {
    for(int y=0; y<256; y++) {
      for(int z=0; z<16; z++) {
        uint8_t block = getBlock(x,y,z);
        if(block == AIR) continue;
        if(getBlock(x+1,y,z) == AIR) {
          addVerticies(0, x, y, z);
        }
        if(getBlock(x-1,y,z) == AIR) {
          addVerticies(1, x, y, z);
        }
        if(getBlock(x,y+1,z) == AIR) {
          addVerticies(2, x, y, z);
        }
        if(getBlock(x,y-1,z) == AIR) {
          addVerticies(3, x, y, z);
        }
        if(getBlock(x,y,z+1) == AIR) {
          addVerticies(4, x, y, z);
        }
        if(getBlock(x,y,z-1) == AIR) {
          addVerticies(5, x, y, z);
        }
      }
    }
  }
  working = false;
  reloadRequired = true;
}

void Chunk::addVerticies(uint32_t side, uint32_t x, uint32_t y, uint32_t z) {
  for(int i = 0; i < 6; i ++) {
    vertexData.push_back(px[side * 6 + i][0] + x);
    vertexData.push_back(px[side * 6 + i][1] + y);
    vertexData.push_back(px[side * 6 + i][2] + z);
    vertexData.push_back(nm[side][0]);
    vertexData.push_back(nm[side][1]);
    vertexData.push_back(nm[side][2]);
    vertexData.push_back(uv[i][0]);
    vertexData.push_back(uv[i][1]);
  }
}

void Chunk::generate() {
  blocks.resize(16*16*256);
  
  const PerlinNoise perlin{123};

  for(int x = 0; x < 16; x++) {
    for(int z = 0; z < 16; z++) {
      int height = perlin.octave2D_01((x * 0.01), (z * 0.01), 4) * 5;
      for(int y = 0; y < 256; y++) {
        if(y <= height)
          setBlock(x, y, z, DIRT);
        else
          setBlock(x, y, z, AIR);
      }
    }
  }
}

}