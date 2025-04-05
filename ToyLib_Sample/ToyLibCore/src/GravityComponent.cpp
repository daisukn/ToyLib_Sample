#include "GravityComponent.h"
#include "Actor.h"
#include "ColliderComponent.h"
#include "BoundingVolumeComponent.h"
#include "PhysWorld.h"
#include "Application.h"
#include <iostream>

GravityComponent::GravityComponent(Actor* a)
    : Component(a)
{
}

void GravityComponent::Update(float deltaTime)
{
    mVelocityY += mGravityAccel * deltaTime;

    Vector3 pos = mOwnerActor->GetPosition();
    pos.y += mVelocityY/9 * deltaTime;
    mOwnerActor->SetPosition(pos); // 仮位置反映

    ColliderComponent* collider = FindFootCollider();
    if (!collider) return;

    PhysWorld* phys = mOwnerActor->GetApp()->GetPhysWorld();
    if (!phys) return;

    float groundY;
    if (phys->GetNearestGroundY(mOwnerActor, groundY))
    {
        std::cout << "groundY"  << groundY << std::endl;
        std::cout << "velocity" << mVelocityY * deltaTime << std::endl;
        const Cube worldBox = collider->GetBoundingVolume()->GetWorldAABB();
        float offset = pos.y - worldBox.min.y;

        if (pos.y - offset <= groundY + 0.01f)  // 少しマージン持たせる
        {
            pos.y = groundY + offset;
            mVelocityY = 0.0f;
            mIsGrounded = true;
            mOwnerActor->SetPosition(pos);
        }
        else
        {
            mIsGrounded = false;
        }
    }
    std::cout << "pos.y" << pos.y << std::endl;

    mOwnerActor->SetPosition(pos);
}

void GravityComponent::Jump()
{
    if (mIsGrounded)
    {
        mVelocityY = mJumpSpeed;
        mIsGrounded = false;
    }
}
ColliderComponent* GravityComponent::FindFootCollider()
{
    for (auto* comp : mOwnerActor->GetAllComponents<ColliderComponent>())
    {
        if (comp->GetColliderType() == C_FOOT)
        {
            return comp;
        }
    }
    return nullptr;
}


