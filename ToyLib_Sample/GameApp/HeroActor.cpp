#include "HeroActor.h"


HeroActor::HeroActor(Application* a)
: Actor(a)
, mAnimID(H_Stand)
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
    float outline = 1.02f;
    JsonHelper::GetBool(json["mesh"], "toon_render", useToon);
    JsonHelper::GetFloat(json["mesh"], "toon_outline", outline);
    meshComp->SetToonRender(useToon, outline);

    // --- Transform設定 ---
    SetPosition(JsonHelper::GetVector3(json, "position"));
    SetRotation(JsonHelper::GetQuaternionFromEuler(json, "rotation_deg"));
    float scale = 1.0f;
    JsonHelper::GetFloat(json, "scale", scale);
    SetScale(scale);

    // --- コライダー ---
    auto cc = CreateComponent<ColliderComponent>();
    cc->GetBoundingVolume()->ComputeBoundingVolume(GetApp()->GetRenderer()->GetMesh(meshPath)->GetVertexArray());
    cc->GetBoundingVolume()->AdjustBoundingBox(
        JsonHelper::GetVector3(json["collider"], "bounding_box_offset"),
        JsonHelper::GetVector3(json["collider"], "bounding_box_scale"));
    cc->GetBoundingVolume()->CreateVArray();
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
    float speed = 5.0f;
      
 

    
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
