#include "Music.h"

Music::Music()
    : mMusic(nullptr)
{
}

Music::~Music()
{
    if (mMusic)
    {
        Mix_FreeMusic(mMusic);
    }
}

bool Music::Load(const std::string& fileName)
{
    mMusic = Mix_LoadMUS(fileName.c_str());
    return mMusic != nullptr;
}

void Music::Play(int loops)
{
    if (mMusic)
    {
        Mix_PlayMusic(mMusic, loops);
    }
}

void Music::Stop()
{
    Mix_HaltMusic();
}
