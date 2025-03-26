#include "MoveComponent.h"
#include "Actor.h"

// コンストラクタ
MoveComponent::MoveComponent(class Actor* a, int updateOrder)
: Component(a, updateOrder)
, mAngularSpeed(0.0f)
, mForwardSpeed(0.0f)
, mRightSpeed(0.0f)
, mDropSpeed(0.0f)
, mUpperSpeed(0.0f)
{
	
}

// マトリックスに適用する
void MoveComponent::Update(float deltaTime)
{
    // 向きを決める　スピードがほぼゼロなら無視
    if (!Math::NearZero(mAngularSpeed))
	{
        // 回転マトリックスを設定
        Quaternion rot = mOwner->GetRotation();
        float angle = mAngularSpeed * deltaTime;

		Quaternion inc(Vector3::UnitY, Math::ToRadians(angle));
		rot = Quaternion::Concatenate(rot, inc);
        mOwner->SetRotation(rot);
	}
	
    
    // スピードがセットされていたら
    if ( !Math::NearZero(mForwardSpeed) )
	{
        // 移動マトリックスを設定
        Vector3 pos = mOwner->GetPosition();
        pos += mOwner->GetForward() * mForwardSpeed * deltaTime;

        mOwner->SetPosition(pos);
        
	}
    
    // スピードがセットされていたら
    if ( !Math::NearZero(mRightSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = mOwner->GetPosition();
        pos += mOwner->GetRight() * mRightSpeed * deltaTime;

        mOwner->SetPosition(pos);
        
    }
    
    // スピードがセットされていたら
    if ( !Math::NearZero(mUpperSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = mOwner->GetPosition();
        pos += mOwner->GetUpward() * mUpperSpeed * deltaTime;

        mOwner->SetPosition(pos);
        
    }
    
    
    if ( !Math::NearZero(mDropSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = mOwner->GetPosition();
        pos += mOwner->GetUpward() * mDropSpeed * deltaTime;
        mOwner->SetPosition(pos);
    }
}

void MoveComponent::Reset()
{
    mAngularSpeed = 0.0f;
    mForwardSpeed = 0.0f;
    mRightSpeed = 0.0f;
    mDropSpeed = 0.0f;
    mUpperSpeed = 0.0f;
}
