#pragma once

#include "Component.h"
#include "Polygon.h"

#include <vector>
#include <memory>

#define ENABLE_BITMASK_OPERATORS(x)                                \
inline x operator|(x a, x b) { return static_cast<x>(static_cast<int>(a) | static_cast<int>(b)); } \
inline x operator&(x a, x b) { return static_cast<x>(static_cast<int>(a) & static_cast<int>(b)); } \
inline x& operator|=(x& a, x b) { a = a | b; return a; }            \
inline x& operator&=(x& a, x b) { a = a & b; return a; }            \
inline x operator~(x a) { return static_cast<x>(~static_cast<int>(a)); }

enum ColliderType : uint32_t
{
    C_NONE    = 0,
    C_PLAYER  = 1 << 0,
    C_ENEMY   = 1 << 1,
    C_BULLET  = 1 << 2,
    C_LASER   = 1 << 3,
    C_WALL    = 1 << 4,
    C_GROUND  = 1 << 5,
    C_FOOT    = 1 << 6,
};

ENABLE_BITMASK_OPERATORS(ColliderType)

/*
// 通知される相手のタイプ
enum ColliderType
{
    C_NONE,     // 
    C_PLAYER,   // プレーヤー
    C_ENEMY,    // 敵
    C_BULLET,   // 弾丸
    C_LASER,
    C_WALL,     // 壁
    C_GROUND,    // 地面
    C_FOOT      // 足元を判定する
};
*/

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
    
    const std::vector<ColliderComponent*>& GetTargetColliders() const { return mTargetColliders; }
    void ClearCollidBuffer() { mTargetColliders.clear(); }
    
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
    std::vector<ColliderComponent*> mTargetColliders;
    
};
