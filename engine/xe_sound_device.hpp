#pragma once

#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/al.h>

namespace xe {

class XeSoundDevice {

  public:

    XeSoundDevice();
    ~XeSoundDevice();

  private:

    void listAudioDevices(const ALCchar* devices);

    ALCdevice* device;
    ALCcontext* context;

};

}