#include "LaserColliderComponent.h"
#include "Actor.h"

LaserColliderComponent::LaserColliderComponent(Actor* a)
: ColliderComponent(a)
, mLength(100.0f) // デフォルトの射程
{
    SetColliderType(C_LASER);
}

Ray LaserColliderComponent::GetRay() const
{
    Vector3 start = mOwner->GetPosition();
    Vector3 dir = mOwner->GetForward(); // 進行方向に伸びる
    return Ray(start, dir);
}
