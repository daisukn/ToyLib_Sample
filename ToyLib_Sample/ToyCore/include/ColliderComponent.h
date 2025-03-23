#pragma once

#include "Component.h"

#include <vector>
#include <memory>

// 通知される相手のタイプ
enum ColliderType
{
    C_NONE,     // 
    C_PLAYER,   // プレーヤー
    C_ENEMY,    // 敵
    C_BULLET,   // 弾丸
    C_LASER
};

class ColliderComponent : public Component
{
public:
    ColliderComponent(class Actor* a);
    virtual ~ColliderComponent();

    // 自分のタイプ
    void SetColliderType(const ColliderType t);
    ColliderType GetColliderType() const { return mType; }
    
    // 衝突した
    void Collided(ColliderComponent* c);
    
    std::vector<ColliderComponent*> GetTargetColliders() const { return targetColliders; }
    void ClearCollidBuffer() { targetColliders.clear(); }
    
    void Update(float deltaTime) override;
    
    class Actor* GetActor() const { return mOwner; }
    class BoundingVolumeComponent* GetBoundingVolume() const { return boundingVolume.get(); }
    
    // 衝突しているかどうか
    bool GetCollided() const { return mIsCollided; }
    void SetCollided(bool b) { mIsCollided = b; }
    
    // 表示されているかどうか
    bool GetDisp() const { return mIsDisp; }
    void SetDisp(bool b) { mIsDisp = b; }

    
private:
    
    bool mIsCollided;
    bool mIsDisp;
    
    std::unique_ptr<class BoundingVolumeComponent> boundingVolume;
    
    ColliderType mType;
    std::vector<ColliderComponent*> targetColliders;
    
};
