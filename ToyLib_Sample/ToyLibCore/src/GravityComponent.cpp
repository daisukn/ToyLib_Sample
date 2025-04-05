#include "GravityComponent.h"
#include "Actor.h"
#include "ColliderComponent.h"
#include "PhysWorld.h"
#include "Application.h"
#include <iostream>

GravityComponent::GravityComponent(Actor* a)
    : Component(a)
{
}

void GravityComponent::Update(float dt)
{
    mVelocityY += mGravityAccel * dt;

    Vector3 pos = mOwnerActor->GetPosition();
    pos.y += mVelocityY * dt;

    float groundY;
    PhysWorld* phys = mOwnerActor->GetApp()->GetPhysWorld();
    if (phys && phys->GetNearestGroundY(mOwnerActor, groundY))
    {
        if (pos.y <= groundY )
        {
            pos.y = groundY;
            mVelocityY = 0.0f;
            mIsGrounded = true;
        }
        else
        {
            mIsGrounded = false;
        }
    }
    else
    {
       std::cout << "No ground detected!" << std::endl;
    }
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
