#include "DirMoveComponent.h"


DirMoveComponent::DirMoveComponent(class Actor* a, int order)
: MoveComponent(a, order)
{
    
}

DirMoveComponent::~DirMoveComponent()
{
    
}

void DirMoveComponent::Update(float deltaTime)
{

}

void DirMoveComponent::ProcessInput(const struct InputState& state)
{
/*
    if(mMovable)
    {
        forwardSpeed = speed * state.Controller.GetLeftStick().y;
        angularSpeed = turn * state.Controller.GetLeftStick().x;
        
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
        {
            angularSpeed = -turn;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
        {
            angularSpeed = turn;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_UP) == EHeld)
        {
            forwardSpeed = speed;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_DOWN) == EHeld)
        {
            forwardSpeed = -speed;
        }
        mAnimID = H_Run;
        if ( forwardSpeed == 0.0f && angularSpeed == 0.0f )
        {
            mAnimID = H_Stand;
        }

    }
*/
}
