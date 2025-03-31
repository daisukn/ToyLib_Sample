#include "FPSMoveComponent.h"
#include "InputSystem.h"


FPSMoveComponent::FPSMoveComponent(class Actor* a, int order)
: MoveComponent(a, order)
, mTurnSpeed(180.f)
, mSpeed(7.f)
{
    
}

FPSMoveComponent::~FPSMoveComponent()
{
    
}


void FPSMoveComponent::ProcessInput(const struct InputState& state)
{

    if(!mIsMovable) return;

    mForwardSpeed = mSpeed * state.Controller.GetLeftStick().y;
    mAngularSpeed = mTurnSpeed * state.Controller.GetLeftStick().x;
        
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
    {
        mAngularSpeed = -mTurnSpeed;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
    {
        mAngularSpeed = mTurnSpeed;
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
