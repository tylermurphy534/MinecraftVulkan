#include "xe_sound.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

namespace xe {

Sound::Sound(const std::string& filename) {

  buffer = alutCreateBufferFromFile(filename.c_str());

  alGenSources(1, &source);

  alSourcef(source, AL_GAIN, 1.f);
  alSourcef(source, AL_PITCH, 1.f);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_BUFFER, buffer);

}

Sound::~Sound() {
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
}

void Sound::play() {
  stop();
  alSourcePlay(source);
};

void Sound::stop() {
  alSourceStop(source);
};

void Sound::pause() {
  alSourcePause(source);
};

void Sound::resume() {
  alSourcePlay(source);
};

bool Sound::isPlaying() {
  ALint playing;
  alGetSourcei(source, AL_SOURCE_STATE, &playing);
  return playing == AL_PLAYING;
};

void Sound::setPosition(glm::vec3 position) {
  alSource3f(source, AL_POSITION, position.x, position.y, position.z);
};

void Sound::setLooping(bool looping) {
  alSourcei(source, AL_LOOPING, looping ? 1 : 0);
};

void Sound::setVolume(float volume) {
  alSourcef(source, AL_GAIN, volume);
}

}