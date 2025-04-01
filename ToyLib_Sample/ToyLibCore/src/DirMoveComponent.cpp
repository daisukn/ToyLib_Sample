#include "DirMoveComponent.h"
#include "InputSystem.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


DirMoveComponent::DirMoveComponent(class Actor* a, int order)
: MoveComponent(a, order)
, mSpeed(7.f)
{
    
}

DirMoveComponent::~DirMoveComponent()
{
    
}

void DirMoveComponent::Update(float deltaTime)
{

    
    // スピードがセットされていたら
    if ( !Math::NearZero(mForwardSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = mOwnerActor->GetPosition();
        Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetZAxis();
        forward.y = 0;
        forward.Normalize();
        pos += forward * mForwardSpeed * deltaTime;

        mOwnerActor->SetPosition(pos);
        
    }
    
    // スピードがセットされていたら
    if ( !Math::NearZero(mRightSpeed) )
    {
        // 移動マトリックスを設定
        Vector3 pos = mOwnerActor->GetPosition();
        Vector3 forward = mOwnerActor->GetApp()->GetRenderer()->GetInvViewMatrix().GetXAxis();
        forward.y = 0;
        forward.Normalize();
        pos += forward * mRightSpeed * deltaTime;

        mOwnerActor->SetPosition(pos);
        
    }
    
    AdjustDir();
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

void DirMoveComponent::AdjustDir()
{
    // 移動ベクトルから向きを調整
    Vector3 moveVal = mOwnerActor->GetPosition() - mOwnerActor->GetPrevPosition();
    
    if (moveVal.x != 0.0f || moveVal.z != 0.0f)
    {
        float rot = Math::Atan2(moveVal.x, moveVal.z);
        Quaternion q = Quaternion(Vector3::UnitY, rot);
        mOwnerActor->SetRotation(q);
    }
}
