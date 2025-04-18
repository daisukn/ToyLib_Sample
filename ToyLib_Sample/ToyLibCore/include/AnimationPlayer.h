#pragma once

#include "MathUtils.h"
#include <vector>
#include <memory>

class AnimationPlayer
{
public:
    AnimationPlayer(std::shared_ptr<class Mesh> mesh);

    void Update(float deltaTime);
    void Play(int animID, bool loop = true);
    void SetPlayRate(float rate) { mPlayRate = rate; }
    void Pause(bool paused) { mIsPaused = paused; }

    const std::vector<Matrix4>& GetFinalMatrices() const { return mFinalMatrices; };
    
    void PlayOnce(int animID, int nextAnimID);
    
    bool IsFinished() const { return mIsFinished; }

private:
    std::shared_ptr<class Mesh> mMesh;
    int mAnimID;
    float mPlayTime;
    float mPlayRate;
    bool mIsLooping;
    bool mIsPaused;
    
    int mNextAnimID;
    bool mIsFinished;

    std::vector<Matrix4> mFinalMatrices;
};

