#include "xe_sound.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

namespace xe {

XeSound::XeSound(const std::string& filename) {

  ALvoid *data;
  ALsizei size, freq;
  ALenum format;
  ALboolean loop;
  char *bufferData;

  // alutLoadWAVFile(filename.c_str(), &format, &data, &size, &freq, &loop);

  // AudioFile<float> file;
  // file.load(filename);

  // std::vector<uint8_t> data;
  // file.writePCMToBuffer(data);
  // auto getALSoundFormat = [](AudioFile<float>& audioFile) {
	// 	int bitDepth = audioFile.getBitDepth();
	// 	if (bitDepth == 16)
	// 		return audioFile.isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
	// 	else if (bitDepth == 8)
	// 		return audioFile.isStereo() ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
	// 	else
	// 		return -1;
	// };

  // alGenBuffers(1, &buffer);
  // alBufferData(buffer, getALSoundFormat(file), data.data(), data.size(), file.getSampleRate());

  alGenSources(1, &source);
  alSourcef(source, AL_GAIN, 1.f);
  alSourcef(source, AL_PITCH, 1.f);
  alSource3f(source, AL_POSITION, 0, 0, 0);
  alSource3f(source, AL_VELOCITY, 0, 0, 0);
  alSourcei(source, AL_LOOPING, AL_FALSE);
  alSourcei(source, AL_BUFFER, buffer);

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