#include "xe_sound.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

namespace xe {

XeSound::XeSound(const std::string& filename) {

  buffer = alutCreateBufferFromFile(filename.c_str());

  alGenSources(1, &source);

  alSourcef(source, AL_GAIN, 1.f);
  alSourcef(source, AL_PITCH, 1.f);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcef(source, AL_GAIN, 3.f);

}

XeSound::~XeSound() {
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
}

void XeSound::play() {
  stop();
  alSourcePlay(source);
};

void XeSound::stop() {
  alSourceStop(source);
};

void XeSound::pause() {
  alSourcePause(source);
};

void XeSound::resume() {
  alSourcePlay(source);
};

bool XeSound::isPlaying() {
  ALint playing;
  alGetSourcei(source, AL_SOURCE_STATE, &playing);
  return playing == AL_PLAYING;
};

void XeSound::setPosition(glm::vec3 position) {
  alSource3f(source, AL_POSITION, position.x, position.y, position.z);
};

void XeSound::setLooping(bool looping) {
  alSourcei(source, AL_LOOPING, looping ? 1 : 0);
};

}