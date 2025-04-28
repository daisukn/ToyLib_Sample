#include "SoundComponent.h"
#include "Actor.h"
#include "Application.h"
#include "AssetManager.h"
#include "SoundEffect.h"
#include "Renderer.h"

SoundComponent::SoundComponent(Actor* owner, int updateOrder)
    : Component(owner, updateOrder)
    , mVolume(1.0f)
    , mIsLoop(false)
    , mAutoPlay(false)
    , mUseDistanceAttenuation(false)
    , mHasPlayed(false)
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
            float actualVolume = mVolume;

            if (mUseDistanceAttenuation)
            {
                // カメラと自分の距離を取る
                Vector3 camPos = mOwnerActor->GetApp()->GetRenderer()->GetViewMatrix().GetTranslation(); // 仮にビュー行列から取る
                Vector3 pos = mOwnerActor->GetPosition();

                float distance = (camPos - pos).Length();
                // 仮減衰モデル（距離が5以上ならボリューム減少）
                if (distance > 5.0f)
                {
                    actualVolume *= (5.0f / distance);
                    if (actualVolume < 0.0f) actualVolume = 0.0f;
                }
            }

            Mix_VolumeChunk(sound->GetChunk(), static_cast<int>(actualVolume * MIX_MAX_VOLUME));
            sound->Play(mIsLoop ? -1 : 0);
        }
    }
}

void SoundComponent::Update(float deltaTime)
{
    if (mAutoPlay && !mHasPlayed)
    {
        Play();
        mHasPlayed = true;
    }
}
