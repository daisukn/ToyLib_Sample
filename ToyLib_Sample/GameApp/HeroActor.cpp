#include "HeroActor.h"


HeroActor::HeroActor(Application* a)
: Actor(a)
, mAnimID(H_Stand)
, mMovable(true)
{
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
    //mMoveComp = CreateComponent<MoveComponent>();
    mMoveComp = CreateComponent<FPSMoveComponent>();
}

HeroActor::~HeroActor()
{

}

void HeroActor::UpdateActor(float deltaTime)
{
    
}

void HeroActor::ActorInput(const InputState &state)
{
    if(mMovable)
    {
        if (mMoveComp->GetForwardSpeed() == 0.0f &&
            mMoveComp->GetAngularSpeed() == 0.0f )
        {
            mAnimID = H_Stand;
        }
        else
        {
            mAnimID = H_Run;
        }
        
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_Z) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_A) == EPressed )
        {
            mAnimID = H_Jump;
            mMovable = false;

        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_X) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_B) == EPressed )
        {
            mAnimID = H_Slash;
            mMovable = false;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_C) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_X) == EPressed )
        {
            mAnimID = H_Spin;
            mMovable = false;
        }
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_V) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_Y) == EPressed )
        {
            mAnimID = H_Stab;
            mMovable = false;
        }

    }
    else
    {
        if (!meshComp->GetIsPlaing())
        {
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
    mMoveComp->SetIsMovable(mMovable);
    
}
