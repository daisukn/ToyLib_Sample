#include "Mesh.h"
#include "AnimationPlayer.h"
#include <assimp/scene.h>
#include <iostream>

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
    // 同じ内容なら再生しなおさない
    if (mAnimID == animID && mIsLooping == loop && !mIsPaused)
        return;

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

    // 範囲外チェック
    const auto& clips = mMesh->GetAnimationClips();
    if (mAnimID < 0 || mAnimID >= static_cast<int>(clips.size()))
    {
        std::cerr << "AnimationPlayer::Update - invalid mAnimID: " << mAnimID << std::endl;
        mIsFinished = true;
        return;
    }

    mIsFinished = false;

    const aiAnimation* anim = clips[mAnimID].mAnimation;
    float ticksPerSecond = clips[mAnimID].mTicksPerSecond;
    float animDuration = clips[mAnimID].mDuration;

    float timeInTicks = mPlayTime * mPlayRate * ticksPerSecond;

    // 再生終了チェック
    if (!mIsLooping && timeInTicks >= animDuration)
    {
        mIsFinished = true;

        if (mNextAnimID >= 0)
        {
            Play(mNextAnimID, true);
        }
        return;
    }

    float animTime = fmod(timeInTicks, animDuration);
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
