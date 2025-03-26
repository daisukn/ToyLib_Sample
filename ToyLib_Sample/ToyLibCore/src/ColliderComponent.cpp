#include "ColliderComponent.h"
#include "Actor.h"
#include "BoundingVolumeComponent.h"
#include "Application.h"
#include "PhysWorld.h"

// コンストラクタ
ColliderComponent::ColliderComponent(Actor* a)
: Component(a)
, mType(C_NONE)
, mIsCollided(false)
, mIsDisp(false)
//, targetType(C_NONE)
{
    boundingVolume = std::make_unique<BoundingVolumeComponent>(a);
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
    targetColliders.emplace_back(c);
    mIsCollided = true;
}


void ColliderComponent::SetColliderType(const ColliderType t)
{
    mType = t;
    mOwner->GetApp()->GetPhysWorld()->AddColliderType(this, t);
    
}
