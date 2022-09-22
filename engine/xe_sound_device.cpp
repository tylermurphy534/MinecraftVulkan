#include "xe_sound_device.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>

namespace xe {

XeSoundDevice::XeSoundDevice() {

  ALboolean enumeration;
  enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_FALSE) {
    fprintf(stderr, "enumeration extension not available\n");
    listAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
  }

  listAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

  const ALCchar* name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

  device = alcOpenDevice(name);
  if (!device) {
    std::runtime_error("failed to get sound device");
  }

  std::cout << "Audio Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << "\n";

  context = alcCreateContext(device, NULL);
  if(!alcMakeContextCurrent(context)) {
    std::runtime_error("failed to make sound context current");
  }

  alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
  alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f);
  ALfloat frontAndUpVectors[] = {
		/* front*/1.f, 0.f, 0.f,
		/* up */ 0.f, 1.f, 0.f
	};
	alListenerfv(AL_ORIENTATION, frontAndUpVectors);

}

void XeSoundDevice::listAudioDevices(const ALCchar* devices) {
  const ALCchar *device = devices, *next = devices + 1;
  size_t len = 0;

  fprintf(stdout, "Devices list: ");
  while (device && *device != '\0' && next && *next != '\0') {
    fprintf(stdout, "%s, ", device);
    len = strlen(device);
    device += (len + 1);
    next += (len + 2);
  }
  std::cout << "\n";
}

XeSoundDevice::~XeSoundDevice() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context);
  alcCloseDevice(device);
}

}