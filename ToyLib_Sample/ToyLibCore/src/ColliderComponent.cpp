#include "ColliderComponent.h"
#include "Actor.h"
#include "BoundingVolumeComponent.h"
#include "Application.h"
#include "PhysWorld.h"

#include <algorithm>

// コンストラクタ
ColliderComponent::ColliderComponent(Actor* a)
: Component(a)
, mType(C_NONE)
, mIsCollided(false)
, mIsDisp(false)
//, targetType(C_NONE)
{
    mBoundingVolume = mOwner->CreateComponent<BoundingVolumeComponent>();
    mOwner->GetApp()->GetPhysWorld()->AddCollider(this);
}

ColliderComponent::~ColliderComponent()
{
    mOwner->GetApp()->GetPhysWorld()->RemoveCollider(this);
}

void ColliderComponent::Update(float deltaTime)
{
    //targetType.clear();
}


// 衝突した
void ColliderComponent::Collided(ColliderComponent* c)
{
    if (std::find(targetColliders.begin(), targetColliders.end(), c) == targetColliders.end())
    {
        targetColliders.emplace_back(c);
        mIsCollided = true;
    }
}

void ColliderComponent::SetColliderType(const ColliderType t)
{
    mType = t;
    mOwner->GetApp()->GetPhysWorld()->AddColliderType(this, t);
    
}
