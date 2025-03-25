#include "CameraComponent.h"
#include "Actor.h"
#include "Renderer.h"
#include "Application.h"
#include "ColliderComponent.h"

CameraComponent::CameraComponent(Actor* a, int updateOrder)
    : Component(a, updateOrder)
{
    mCameraActor = std::make_unique<Actor>(mOwner->GetApp());
}

void CameraComponent::SetViewMatrix(const Matrix4& view)
{
	mOwner->GetApp()->GetRenderer()->SetViewMatrix(view);
}

void CameraComponent::SetCameraPosition(const Vector3 pos)
{
    mOwner->GetApp()->GetRenderer()->SetCameraPosition(pos);
}

void CameraComponent::Update(float deltaTime)
{
    auto inView = mOwner->GetApp()->GetRenderer()->GetInvViewMatrix();
    mCameraPosition = inView.GetTranslation();
}


