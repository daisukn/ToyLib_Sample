#include "OrbitCamera.h"
#include "Actor.h"
#include "ColliderComponent.h"
#include "InputSystem.h"

OrbitCamera::OrbitCamera(Actor* actor)
    : CameraComponent(actor)
    , mOffset(-0.0f, 2.0f, -10.0f)
    , mUpVector(Vector3::UnitY)
    , mPitchSpeed(0.0f)
    , mYawSpeed(0.0f)
{
}

void OrbitCamera::ProcessInput( const struct InputState& state )
{
    float angularSpeed = 2.0f;
    
    SetYawSpeed( angularSpeed * state.Controller.GetRightStick().x );
    ChangeHeight( angularSpeed * state.Controller.GetRightStick().y );


    // キーボード（カメラ）
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_D) == EHeld)
    {
        SetYawSpeed( angularSpeed );
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_A) == EHeld)
    {
        SetYawSpeed( -angularSpeed );
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_W) == EHeld)
    {
        ChangeHeight(angularSpeed);
    }
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_S) == EHeld)
    {
        ChangeHeight(-angularSpeed);
    }
    
}

void OrbitCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// ヨー回転のクォータニオン
	Quaternion yaw(Vector3::UnitY, mYawSpeed * deltaTime);
	// オフセットを計算
    mOffset = Vector3::Transform(mOffset, yaw);
	mUpVector = Vector3::Transform(mUpVector, yaw);

	// カメラのforwardとrightを計算
	// Forward owner.position - (owner.position + offset) = -offset
    Vector3 forward = -1.0f * mOffset;
	forward.Normalize();
	Vector3 right = Vector3::Cross(mUpVector, forward);
	right.Normalize();
	
	// ピッチ回転のクォータニオン
	Quaternion pitch(right, mPitchSpeed * deltaTime);
	// オフセットを計算
	mOffset = Vector3::Transform(mOffset, pitch);
	mUpVector = Vector3::Transform(mUpVector, pitch);

	// Ownerを向くビューマトリックスを生成
    Vector3 target = mOwnerActor->GetPosition() + Vector3(0, 1, 0);
	Vector3 cameraPos = target + mOffset;
    
    
    mOffset.y += mChangeOffset;
    
    // 地表に合わせる
    if(cameraPos.y < 0)
    {
        cameraPos.y = 0;
        if( mChangeOffset < 0.0f)
        {
            mOffset.y -= mChangeOffset;
        }
        
    }
    if(cameraPos.y > 10)
    {
        cameraPos.y = 10;
        if( mChangeOffset > 10.0f)
        {
            mOffset.y += mChangeOffset;
        }
        
    }
    mCameraPosition = cameraPos;
     

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUpVector);
	SetViewMatrix(view);
    
    
    mCameraActor->SetPosition(cameraPos);
}
