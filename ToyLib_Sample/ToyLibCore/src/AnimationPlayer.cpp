#include "Mesh.h"
#include "AnimationPlayer.h"
#include <assimp/scene.h>

AnimationPlayer::AnimationPlayer(std::shared_ptr<Mesh> mesh)
: mMesh(mesh)
, mAnimID(0)
, mPlayTime(0.0f)
, mPlayRate(1.0f)
, mIsLooping(true)
, mIsPaused(false)
, mNextAnimID(-1)
, mIsFinished(false)
{
}

void AnimationPlayer::Play(int animID, bool loop)
{
    mAnimID = animID;
    mPlayTime = 0.0f;
    mIsLooping = loop;
    mIsPaused = false;
    mIsFinished = false;
    mNextAnimID = -1;
}

void AnimationPlayer::Update(float deltaTime)
{
    if (!mMesh || mIsPaused) return;
    
    const aiAnimation* anim = mMesh->GetScene()->mAnimations[mAnimID];
    float ticksPerSecond = (anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);
    float timeInTicks = mPlayTime * mPlayRate * ticksPerSecond;

    float animTime = fmod(timeInTicks, anim->mDuration);

    // 再生終了チェック
    if (!mIsLooping && mPlayTime * mPlayRate * ticksPerSecond >= anim->mDuration)
    {
        mIsFinished = true;
        if (mNextAnimID >= 0)
        {
            Play(mNextAnimID, true); // 次にループするやつに切り替え
        }
        return;
    }
    
    if (anim->mDuration <= 0.0f)
    {
        mIsFinished = true;
        return;
    }
    
    mMesh->ComputePoseAtTime(animTime, anim, mFinalMatrices);
    mPlayTime += deltaTime;
}
void AnimationPlayer::PlayOnce(int animID, int nextAnimID)
{
    mAnimID = animID;
    mPlayTime = 0.0f;
    mIsLooping = false;
    mIsPaused = false;
    mIsFinished = false;
    mNextAnimID = nextAnimID;
}
