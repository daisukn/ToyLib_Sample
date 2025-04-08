#include "DirMoveComponent.h"
#include "InputSystem.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "PhysWorld.h"


DirMoveComponent::DirMoveComponent(class Actor* a, int order)
: MoveComponent(a, order)
, mSpeed(9.f)
, mPrevPosition(Vector3::Zero)
{
    
}

DirMoveComponent::~DirMoveComponent()
{
    
}
/*
void DirMoveComponent::Update(float deltaTime)
{
    // 移動マトリックスを設定
    Vector3 pos = mOwnerActor->GetPosition();
    // スピードがセットされていたら
    if (!Math::NearZero(mForwardSpeed))
    {
        Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetZAxis();
        forward.y = 0;
        forward.Normalize();
        pos += forward * mForwardSpeed * deltaTime;
    }
    
    // スピードがセットされていたら
    if (!Math::NearZero(mRightSpeed))
    {
        Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetXAxis();
        forward.y = 0;
        forward.Normalize();
        pos += forward * mRightSpeed * deltaTime;
    }
    
    mOwnerActor->SetPosition(pos);
    //if (mIsTurnable)
    {
        AdjustDir();
    }
    
    mPrevPosition = mOwnerActor->GetPosition();
}
 */
 
void DirMoveComponent::Update(float deltaTime)
{
    //Vector3 pos = mOwnerActor->GetPosition();

    // カメラ基準の移動方向を計算
    Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetZAxis();
    Vector3 right = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetXAxis();

    forward.y = 0.0f;
    right.y = 0.0f;
    forward.Normalize();
    right.Normalize();

    Vector3 moveDir = forward * mForwardSpeed + right * mRightSpeed;

    if (moveDir.LengthSq() > Math::NearZeroEpsilon)
    {
        moveDir.Normalize();
        moveDir.Normalize();
        TryMoveWithRayCheck(moveDir * mSpeed, deltaTime);
    }

    AdjustDir();
    mPrevPosition = mOwnerActor->GetPosition();
}

void DirMoveComponent::ProcessInput(const struct InputState& state)
{

    if(!mIsMovable) return;


    mForwardSpeed = mSpeed * state.Controller.GetLeftStick().y;
    mRightSpeed = mSpeed * state.Controller.GetLeftStick().x;
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
    {
        mRightSpeed = -mSpeed;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
    {
        mRightSpeed = mSpeed;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_UP) == EHeld)
    {
        mForwardSpeed = mSpeed;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_DOWN) == EHeld)
    {
        mForwardSpeed = -mSpeed;
    }

}
/*
void DirMoveComponent::AdjustDir()
{
    // 移動ベクトルから向きを調整
    Vector3 moveVal = mOwnerActor->GetPosition() - mPrevPosition;
    
    // ノイズ除去
    if (moveVal.LengthSq() > 0.0001f)
    {
        float rot = Math::Atan2(moveVal.x, moveVal.z);
        Quaternion q = Quaternion(Vector3::UnitY, rot);
        mOwnerActor->SetRotation(q);
    }
}
*/
/*
void DirMoveComponent::AdjustDir()
{
    // カメラ前方向に向きを合わせる
    Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetZAxis();
    forward.y = 0;
    if (forward.LengthSq() > 0.0001f)
    {
        forward.Normalize();
        float rot = Math::Atan2(forward.x, forward.z);
        Quaternion q = Quaternion(Vector3::UnitY, rot);
        mOwnerActor->SetRotation(q);
    }
}
*/
void DirMoveComponent::AdjustDir()
{
    Vector3 moveVal = mOwnerActor->GetPosition() - mPrevPosition;
    moveVal.y = 0.f;
    if (moveVal.LengthSq() > 0.01f)
    {
        float rot = Math::Atan2(moveVal.x, moveVal.z);
        Quaternion targetRot = Quaternion(Vector3::UnitY, rot);
        Quaternion currentRot = mOwnerActor->GetRotation();

        // 球面線形補間（スムーズな回転）
        Quaternion smoothRot = Quaternion::Slerp(currentRot, targetRot, 0.1f); // 0.1fは回転の追従速度
        mOwnerActor->SetRotation(smoothRot);
    }
}
