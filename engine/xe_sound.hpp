#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace xe {

class XeSound {

	public:

		XeSound(const std::string& filename);
		~XeSound();

		void play();
		void stop();
		void pause();
		void resume();

		bool isPlaying();

		void setPosition(glm::vec3 position);
		void setLooping(bool looping);

	private:
		ALuint source;
		ALuint buffer;
		ALenum format;

};

}