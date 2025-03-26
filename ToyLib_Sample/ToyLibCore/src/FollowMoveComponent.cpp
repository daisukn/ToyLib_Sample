#include "FollowMoveComponent.h"
#include "Actor.h"
#include "MathUtils.h"

FollowMoveComponent::FollowMoveComponent(Actor* owner, int updateOrder)
    : MoveComponent(owner, updateOrder)
    , mTarget(nullptr)
    , mFollowDistance(300.0f)
    , mFollowSpeed(150.0f)
    , mRotationSpeed(90.0f)
{
}

void FollowMoveComponent::Update(float deltaTime)
{
    MoveComponent::Update(deltaTime);

    if (mTarget)
    {
        Vector3 toTarget = mTarget->GetPosition() - mOwner->GetPosition();
        float dist = toTarget.Length();

        if (dist > Math::NearZeroEpsilon)
        {
            toTarget.Normalize();

            // 向きを合わせる
            Vector3 forward = mOwner->GetForward();
            float dot = Vector3::Dot(forward, toTarget);
            dot = Math::Clamp(dot, -1.0f, 1.0f);
            float angle = Math::Acos(dot);

            if (angle > Math::ToRadians(1.0f))
            {
                float maxRot = mRotationSpeed * deltaTime;
                if (angle > maxRot)
                    angle = maxRot;

                Vector3 cross = Vector3::Cross(forward, toTarget);
                if (cross.y < 0)
                    angle = -angle;

                Quaternion rot = mOwner->GetRotation();
                Quaternion inc(Vector3::UnitY, angle);
                rot = Quaternion::Concatenate(rot, inc);
                mOwner->SetRotation(rot);
            }

            // 距離が一定以上なら前進
            if (dist > mFollowDistance)
            {
                Vector3 pos = mOwner->GetPosition();
                pos += mOwner->GetForward() * mFollowSpeed * deltaTime;
                mOwner->SetPosition(pos);
            }
        }
    }
}
