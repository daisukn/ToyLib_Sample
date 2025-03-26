#include "MoveComponent.h"
#include "Actor.h"

// コンストラクタ
MoveComponent::MoveComponent(class Actor* a, int updateOrder)
: Component(a, updateOrder)
, mAngularSpeed(0.0f)
, mForwardSpeed(0.0f)
, mRightSpeed(0.0f)
, mVerticalSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
    Quaternion rot = mOwner->GetRotation();
    if (!Math::NearZero(mAngularSpeed))
    {
        float angle = Math::ToRadians(mAngularSpeed * deltaTime);
        Quaternion inc(Vector3::UnitY, angle);
        rot = Quaternion::Concatenate(rot, inc);
        mOwner->SetRotation(rot);
    }

    Vector3 pos = mOwner->GetPosition();
    if (!Math::NearZero(mForwardSpeed))
        pos += mOwner->GetForward() * mForwardSpeed * deltaTime;

    if (!Math::NearZero(mRightSpeed))
        pos += mOwner->GetRight() * mRightSpeed * deltaTime;

    if (!Math::NearZero(mVerticalSpeed))
        pos += mOwner->GetUpward() * mVerticalSpeed * deltaTime;

    mOwner->SetPosition(pos);
}

void MoveComponent::Reset()
{
    mAngularSpeed = 0.0f;
    mForwardSpeed = 0.0f;
    mRightSpeed = 0.0f;
    mVerticalSpeed = 0.0f;
}
