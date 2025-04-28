#include "SoundComponent.h"
#include "Actor.h"
#include "Application.h"
#include "AssetManager.h"
#include "SoundEffect.h"

SoundComponent::SoundComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
    , mVolume(1.0f)
    , mIsLoop(false)
{
}

SoundComponent::~SoundComponent()
{
}

void SoundComponent::SetSound(const std::string& fileName)
{
    mSoundName = fileName;
}

void SoundComponent::Play()
{
    if (!mSoundName.empty())
    {
        auto sound = mOwnerActor->GetApp()->GetAssetManager()->GetSoundEffect(mSoundName);
        if (sound)
        {
            Mix_VolumeChunk(sound->GetChunk(), static_cast<int>(mVolume * MIX_MAX_VOLUME));
            sound->Play(mIsLoop ? -1 : 0);
        }
    }
}
