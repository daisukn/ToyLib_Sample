#pragma once

#include "Component.h"
#include "Polygon.h"

#include <vector>
#include <memory>

// 通知される相手のタイプ
enum ColliderType
{
    C_NONE,     // 
    C_PLAYER,   // プレーヤー
    C_ENEMY,    // 敵
    C_BULLET,   // 弾丸
    C_LASER,
    C_WALL,     // 壁
    C_GROUND    // 地面
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
    
    const std::vector<ColliderComponent*>& GetTargetColliders() const { return targetColliders; }
    void ClearCollidBuffer() { targetColliders.clear(); }
    
    void Update(float deltaTime) override;
    
    //class Actor* GetActor() const { return mOwner; }
    class BoundingVolumeComponent* GetBoundingVolume() const { return mBoundingVolume; }
    
    // 衝突しているかどうか
    bool GetCollided() const { return mIsCollided; }
    void SetCollided(bool b) { mIsCollided = b; }
    
    // 表示されているかどうか
    bool GetDisp() const { return mIsDisp; }
    void SetDisp(bool b) { mIsDisp = b; }

    // レイを取得 基本は空
    virtual Ray GetRay() const { return Ray(); }
    
private:
    
    bool mIsCollided;
    bool mIsDisp;
    
    class BoundingVolumeComponent* mBoundingVolume;
    
    ColliderType mType;
    std::vector<ColliderComponent*> targetColliders;
    
};
