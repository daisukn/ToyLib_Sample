#pragma once

#include "Actor.h"

#ifndef __HEROMOTION
#define __HEROMOTION
enum HeroMotion
{
    H_Run = 11,
    H_Dead = 0,
    H_Guard = 1,
    H_Jump = 4,
    H_Stand = 17,
    H_Walk = 18,
    H_Slash = 13,
    H_Spin = 14,
    H_Stab = 15
    
};
#endif


class HeroActor : public Actor
{
public:
    HeroActor(class Application* a);
    virtual ~HeroActor();
    void UpdateActor(float deltaTime) override;
    void ActorInput(const struct InputState& state);
private:
    enum HeroMotion mAnimID;
    class MoveComponent* mMoveComp;
    class SkeletalMeshComponent* meshComp;
};
