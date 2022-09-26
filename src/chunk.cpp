#include "chunk.hpp"
#include "xe_engine.hpp"

namespace app {

static std::map<std::pair<int32_t, int32_t>, Chunk*> chunks{};

Chunk::Chunk(int32_t gridX, int32_t gridZ, uint32_t world_seed) 
  : world_seed{world_seed},
    chunk_seed{(world_seed * gridX) + (world_seed * gridZ) / 2},
    gridX{gridX},
    gridZ{gridZ} {
  generate();
}

Chunk* Chunk::newChunk(int32_t gridX, int32_t gridZ, uint32_t world_seed) {
  Chunk* chunk = new Chunk(gridX, gridZ, world_seed);
  chunks[{gridX, gridZ}] = std::move(chunk);
  return chunks[{gridX, gridZ}];
}

Chunk* Chunk::getChunk(int32_t gridX, int32_t gridZ) {
  if(chunks.count({gridX, gridZ})) {
    return chunks[{gridX, gridZ}];
  } else {
    return NULL;
  }
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
    return blocks[index];
  } else {
    Chunk* chunk = getChunk(chunkX, chunkZ);
    if(chunk == NULL) {
      return INVALID;
    } else {
      int index = x + (z * 16) + (y * 256);
      return chunk->blocks[index];
    }
  }
}

void Chunk::setBlock(int32_t x, int32_t y, int32_t z, uint8_t block) {
  int index = x + (z * 16) + (y * 256);
  blocks[index] = block;
}

void Chunk::reset() {
  for(const auto &[key, chunk]: chunks) {
    delete chunk;
  }
  chunks.clear();
}

std::shared_ptr<xe::Model> Chunk::getMesh() {
  if(reloadRequired) {
    delete chunkMesh.get();
    xe::Model::Builder builder{};
    builder.vertexData = vertexData;
    builder.vertexSize = 36;
    chunkMesh = std::make_shared<xe::Model>(xe::Engine::getInstance()->getDevice(), builder);
  }
  return chunkMesh;
}

void Chunk::createMeshAsync() {
  if(working) return;
  // worker = std::thread(createMesh);
}

void Chunk::createMesh() {
  working = true;
  vertexData.clear();
  for(int32_t x=0;x<16;x++) {
    for(int32_t y=0; y<256; y++) {
      for(int32_t z=0; z<16; z++) {
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

void Chunk::addVerticies(uint8_t side, int32_t x, int32_t y, int32_t z) {
  for(int i = 0; i < 6; i ++) {
    vertexData.push_back(px[side * 6 + i][0] + x);
    vertexData.push_back(px[side * 6 + i][1] + y);
    vertexData.push_back(px[side * 6 + i][2] + z);
    vertexData.push_back(nm[side][0]);
    vertexData.push_back(nm[side][1]);
    vertexData.push_back(nm[side][2]);
    vertexData.push_back(uv[i][0]);
    vertexData.push_back(uv[i][1]);
    vertexData.push_back(0.f);
  }
}

void Chunk::generate() {
  blocks.resize(16*16*256);
  
  const PerlinNoise perlin{world_seed};

  for(int x = 0; x < 16; x++) {
    for(int z = 0; z < 16; z++) {
      int height = perlin.octave2D_01((( x + gridX * 16) * 0.01), ((z + gridZ * 16) * 0.01), 4) * 10;
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