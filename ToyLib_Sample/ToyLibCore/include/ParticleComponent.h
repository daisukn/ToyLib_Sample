#pragma once

#include "Component.h"
#include "MathUtils.h"

#include <vector>

// パーティクルの破片
struct ParticleParts
{

    bool isVisible;
    Vector3 pos;
    Vector3 dir;
    float size;
    float lifeTime;

    
    ParticleParts()
        : pos(Vector3(0.0f, 0.0f, 0.0f))
        , dir(Vector3(0.0f, 0.0f, 0.0f))
        , isVisible(false)
        , lifeTime(0.0f)
        , size(0.0)

    {}
};


// パーティクルコンポーネント（SpriteComponentから継承した方が良いかもしれない）
class ParticleComponent : public Component
{
public:
    enum ParticleMode
    {
        P_SPARK,
        P_SMOKE,
        P_WATER
    };
    
    
    ParticleComponent(class Actor* a, int drawOrder = 100);
    ~ParticleComponent();
    
    // Rendererから呼ばれる
    virtual void Draw(class Shader* shader);

    // テクスチャセット
    virtual void SetTexture(class Texture* tex);
    
    // アップデート処理
    void Update(float deltaTime) override;

    // パーティクル発生
    void CreateParticles(Vector3 pos, unsigned int num, float life, float part_life, float size, ParticleMode mode = P_SPARK);
    
    
    bool GetVisible() const { return mIsVisible; }
    void SetVisible(bool b) { mIsVisible = b; }
    int GetDrawOrder() const { return mDrawOrder; }
    
    void SetParticleSpeed(float f) { mPartSpeed = f; }
    float GetParticleSpeed() const { return mPartSpeed; }

    
    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool GetBlendAdd() const { return mIsBlendAdd; }
private:
    // 所有アクターとの相対位置
    Vector3 mPosition;
    bool mIsVisible;
    unsigned int mNumParts;
    float mLifeTime;
    float mTotalLife;
    float mPartLifecycle;
    float mPartSize;

    float mPartSpeed;

    ParticleMode mParticleMode;
    //bool isGravity;
    int mDrawOrder;
    // パーティクル破片を生成
    void GenerateParts();

    std::vector<ParticleParts> mParts;
    
    class Texture* mTexture;
    
    // 加算合成するか
    bool mIsBlendAdd;
    

}; 


