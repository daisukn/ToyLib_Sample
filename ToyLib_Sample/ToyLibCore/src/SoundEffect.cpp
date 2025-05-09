#include "SoundEffect.h"

SoundEffect::SoundEffect()
    : mChunk(nullptr)
{
}

SoundEffect::~SoundEffect()
{
    if (mChunk)
    {
        Mix_FreeChunk(mChunk);
    }
}

bool SoundEffect::Load(const std::string& fileName)
{
    mChunk = Mix_LoadWAV(fileName.c_str());
    return mChunk != nullptr;
}

void SoundEffect::Play(int loops)
{
    if (mChunk)
    {
        Mix_PlayChannel(-1, mChunk, loops);
    }
}

