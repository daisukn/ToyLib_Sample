#include "HeroActor.h"


HeroActor::HeroActor(Application* a)
: Actor(a)
, mAnimID(H_Stand)
, mMovable(true)
{
    /*
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
    */
    
    // --- JSON読み込み ---
    std::ifstream file("Settings/HeroActor.json");
    nlohmann::json json;
    file >> json;

    // --- スケルタルメッシュ ---
    meshComp = CreateComponent<SkeletalMeshComponent>();
    std::string meshPath;
    if (json.contains("mesh") && json["mesh"].contains("file"))
    {
        JsonHelper::GetString(json["mesh"], "file", meshPath);
    }
    meshComp->SetMesh(a->GetRenderer()->GetMesh(meshPath));
    meshComp->SetAnimID(mAnimID, PLAY_CYCLIC);

    bool useToon = false;
    float outline = 1.00f;
    JsonHelper::GetBool(json["mesh"], "toon_render", useToon);
    JsonHelper::GetFloat(json["mesh"], "toon_outline", outline);
    meshComp->SetToonRender(useToon, outline);

    // --- Transform設定 ---
    Vector3 pos;
    JsonHelper::GetVector3(json, "position", pos);
    SetPosition(pos);
    Quaternion q;
    JsonHelper::GetQuaternionFromEuler(json, "rotation_deg", q);
    SetRotation(q);
    float scale = 1.0f;
    JsonHelper::GetFloat(json, "scale", scale);
    SetScale(scale);

    // --- コライダー ---
    auto cc = CreateComponent<ColliderComponent>();
    cc->GetBoundingVolume()->ComputeBoundingVolume(GetApp()->GetRenderer()->GetMesh(meshPath)->GetVertexArray());
    Vector3 vOffset;
    JsonHelper::GetVector3(json["collider"], "bounding_box_offset", vOffset);
    Vector3 vScale;
    JsonHelper::GetVector3(json["collider"], "bounding_box_scale", vScale);
    cc->GetBoundingVolume()->AdjustBoundingBox(vOffset, vScale);
    cc->GetBoundingVolume()->SetVisible(true);
    cc->SetColliderType(C_PLAYER);
    cc->SetDisp(true);

    // --- 移動コンポーネント ---
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
    float speed = 7.0f;
    float turn = 180.f;
      
 
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
    

    if(mMovable)
    {
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_Z) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_A) == EPressed )
        {
            mMovable = false;
            mAnimID = H_Jump;

        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_X) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_B) == EPressed )
        {
            mMovable = false;
            mAnimID = H_Slash;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_C) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_X) == EPressed )
        {
            mMovable = false;
            mAnimID = H_Spin;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_V) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_Y) == EPressed )
        {
            mMovable = false;
            mAnimID = H_Stab;
        }
        
    }
    else
    {
        if ( !meshComp->GetIsPlaing() )
        {
            if (mAnimID == H_Slash)
            {


            }
            mMovable = true;
        }

    }
    

    if (mAnimID == H_Run || mAnimID == H_Stand)
    {
        meshComp->SetAnimID(mAnimID, PLAY_CYCLIC);
    }
    else
    {
        meshComp->SetAnimID(mAnimID, PLAY_ONCE);
    }

    mMoveComp->SetAngularSpeed(angularSpeed);
    mMoveComp->SetForwardSpeed(forwardSpeed);
    
    
}
