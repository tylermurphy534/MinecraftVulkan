#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace xe {

class Sound {

	public:

		Sound(const std::string& filename);
		~Sound();

		void play();
		void stop();
		void pause();
		void resume();

		bool isPlaying();

		void setPosition(glm::vec3 position);
		void setLooping(bool looping);
		void setVolume(float volume);

	private:
		ALuint source;
		ALuint buffer;
		ALenum format;

};

}