#pragma once

#include "MathUtils.h"
#include "MeshComponent.h"
#include "Animation.h"

#include <vector>

const size_t MAX_SKELETON_BONES = 96;

// スケルタルアニメーションメッシュを管理
class SkeletalMeshComponent : public MeshComponent
{
public:
    // コンストラクタ
    SkeletalMeshComponent(class Actor* owner);
    
    // 描画のOverride
    void Draw(class Shader* shader) override;
    // UpdateのOverride
    void Update(float deltaTime) override;
    
    // 再生するモーションのID
    virtual void SetAnimID(const unsigned int animID, const PlayMode mode) override;
    
    bool GetIsPlaing() const;

private:

    
    // アニメーション時間
    float mAnimTime;
    

};

