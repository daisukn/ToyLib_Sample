#pragma once
#include "Component.h"
#include <string>

class SoundComponent : public Component
{
public:
    SoundComponent(class Actor* owner, int updateOrder = 100);
    ~SoundComponent();

    void SetSound(const std::string& fileName);
    void Play();

    void SetVolume(float volume) { mVolume = volume; }
    void SetLoop(bool loop) { mIsLoop = loop; }

private:
    std::string mSoundName;
    float mVolume;
    bool mIsLoop;
};
