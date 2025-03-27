#include "HeroActor.h"


HeroActor::HeroActor(Application* a)
: Actor(a)
, mAnimID(H_Stand)
{
    meshComp = CreateComponent<SkeletalMeshComponent>();
    meshComp->SetMesh(GetApp()->GetRenderer()->GetMesh("Assets/hero_m.fbx"));
    meshComp->SetAnimID(mAnimID, PLAY_CYCLIC);
    meshComp->SetToonRender(false, 1.02);
    
    // 場所調整
    SetPosition(Vector3(0.0f, -2.f, 10.0f));
    SetScale(0.001);
    Quaternion q = Quaternion(Vector3::UnitY, Math::ToRadians(180));
    SetRotation(q);
    
    auto cc = CreateComponent<ColliderComponent>();
    cc->GetBoundingVolume()->ComputeBoundingVolume(GetApp()->GetRenderer()->GetMesh("Assets/hero_m.fbx")->GetVertexArray());
    cc->GetBoundingVolume()->AdjustBoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5, 1.f, 0.4));
    cc->GetBoundingVolume()->CreateVArray();
    cc->GetBoundingVolume()->SetVisible(true);
    cc->SetColliderType(C_PLAYER);
    cc->SetDisp(true);
    
    mMoveComp = CreateComponent<MoveComponent>();
    
}

HeroActor::~HeroActor()
{

}

void HeroActor::UpdateActor(float deltaTime)
{
    
}

void HeroActor::ActorInput(const InputState &state)
{
    float forwardSpeed = 0.0f;
    float angularSpeed = 0.0f;
    float speed = 10.0f;
      
 

    
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_LEFT) == EHeld)
    {
        angularSpeed = -100;
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_RIGHT) == EHeld)
    {
        angularSpeed = 100;
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
    meshComp->SetAnimID(mAnimID, PLAY_CYCLIC);
    mMoveComp->SetAngularSpeed(angularSpeed);
    mMoveComp->SetForwardSpeed(forwardSpeed);
    
    
}
