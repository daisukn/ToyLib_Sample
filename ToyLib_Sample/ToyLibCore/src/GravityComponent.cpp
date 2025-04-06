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
    // 1. 重力加速度を加算
    mVelocityY += mGravityAccel * deltaTime;

    // 2. 仮の位置へ移動
    Vector3 pos = mOwnerActor->GetPosition();
    pos.y += mVelocityY/9 * deltaTime;
    //mOwnerActor->SetPosition(pos); // 仮反映

    // 3. 接地判定に必要な情報を取得
    ColliderComponent* collider = FindFootCollider();
    if (!collider) return;

    PhysWorld* phys = mOwnerActor->GetApp()->GetPhysWorld();
    if (!phys) return;

    // 4. 地面の高さを取得
    float groundY = 0.0f;
    if (phys->GetNearestGroundY(mOwnerActor, mVelocityY * deltaTime, groundY))
    {
        std::cout << "groundY=" << groundY << std::endl;
        const Cube worldBox = collider->GetBoundingVolume()->GetWorldAABB();
        float footY = worldBox.min.y;

        //float gap = footY - groundY;
        float gap = worldBox.min.y - groundY;
        if (gap <= 0.0f)
        {
        // ログで確認
        //std::cout << "[判定] footY: " << footY << ", groundY: " << groundY << ", gap: " << gap << std::endl;

        // 5. 地面に近づいたら停止
        //if (gap <= 0.05f && mVelocityY <= 0.0f)
        //{
            float offset = pos.y - footY;
            pos.y = groundY + offset;
            mOwnerActor->SetPosition(pos); // 最終位置再設定
            mVelocityY = 0.0f;
            mIsGrounded = true;

            //std::cout << "[着地] pos.y = " << pos.y << std::endl;
            return;
        }
    }

    // 6. それ以外は空中扱い
    mIsGrounded = false;
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


