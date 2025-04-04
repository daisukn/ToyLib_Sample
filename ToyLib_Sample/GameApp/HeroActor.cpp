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
    mMeshComp = CreateComponent<SkeletalMeshComponent>();
    std::string meshPath;
    if (json.contains("mesh") && json["mesh"].contains("file"))
    {
        JsonHelper::GetString(json["mesh"], "file", meshPath);
    }
    mMeshComp->SetMesh(a->GetRenderer()->GetMesh(meshPath));
    mMeshComp->SetAnimID(mAnimID, PLAY_CYCLIC);

    bool useToon = false;
    float outline = 1.00f;
    JsonHelper::GetBool(json["mesh"], "toon_render", useToon);
    JsonHelper::GetFloat(json["mesh"], "toon_outline", outline);
    mMeshComp->SetToonRender(useToon, outline);

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
    mCollComp = CreateComponent<ColliderComponent>();
    mCollComp->GetBoundingVolume()->ComputeBoundingVolume(GetApp()->GetRenderer()->GetMesh(meshPath)->GetVertexArray());
    Vector3 vOffset;
    JsonHelper::GetVector3(json["collider"], "bounding_box_offset", vOffset);
    Vector3 vScale;
    JsonHelper::GetVector3(json["collider"], "bounding_box_scale", vScale);
    mCollComp->GetBoundingVolume()->AdjustBoundingBox(vOffset, vScale);
    mCollComp->GetBoundingVolume()->SetVisible(true);
    mCollComp->SetColliderType(C_PLAYER);
    mCollComp->SetDisp(true);

    // --- 移動コンポーネント ---
    //mMoveComp = CreateComponent<MoveComponent>();
    mMoveComp = CreateComponent<FPSMoveComponent>();
    //mMoveComp = CreateComponent<DirMoveComponent>();
    
    
    // --- カメラコンポーネント ---
    //mCameraComp = CreateComponent<OrbitCameraComponent>();
    mCameraComp = CreateComponent<FollowCameraComponent>();
}

HeroActor::~HeroActor()
{

}

void HeroActor::UpdateActor(float deltaTime)
{
    
}

void HeroActor::ActorInput(const InputState& state)
{
    bool inputAttack = false;

    if (mMovable)
    {
        // 攻撃・ジャンプ入力判定
        if (state.Keyboard.GetKeyState(SDL_SCANCODE_Z) == EPressed ||
            state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_A) == EPressed)
        {
            mAnimID = H_Jump;
            inputAttack = true;
        }
        else if (state.Keyboard.GetKeyState(SDL_SCANCODE_X) == EPressed ||
                 state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_B) == EPressed)
        {
            mAnimID = H_Slash;
            inputAttack = true;
        }
        else if (state.Keyboard.GetKeyState(SDL_SCANCODE_C) == EPressed ||
                 state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_X) == EPressed)
        {
            mAnimID = H_Spin;
            inputAttack = true;
        }
        else if (state.Keyboard.GetKeyState(SDL_SCANCODE_V) == EPressed ||
                 state.Controller.GetButtonState(SDL_CONTROLLER_BUTTON_Y) == EPressed)
        {
            mAnimID = H_Stab;
            inputAttack = true;
        }

        // 攻撃入力があれば移動ロック
        if (inputAttack)
        {
            mMovable = false;
        }
        else
        {
            // 攻撃入力なし → 通常移動状態に応じたアニメ設定
            if (mMoveComp->GetForwardSpeed() == 0.0f &&
                mMoveComp->GetAngularSpeed() == 0.0f &&
                mMoveComp->GetRightSpeed() == 0.0f)
            {
                mAnimID = H_Stand;
            }
            else
            {
                mAnimID = H_Run;
            }
        }
    }
    else
    {
        // 攻撃アニメーション終了で移動再開
        if (!mMeshComp->GetIsPlaing())
        {
            mMovable = true;
        }
    }

    // アニメーション再生
    mMeshComp->SetAnimID(mAnimID,
        (mAnimID == H_Run || mAnimID == H_Stand) ? PLAY_CYCLIC : PLAY_ONCE);

    // 移動ロックをMoveComponentに伝える
    mMoveComp->SetIsMovable(mMovable);
}
