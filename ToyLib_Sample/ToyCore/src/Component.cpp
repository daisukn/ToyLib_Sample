#include "Component.h"
#include "Actor.h"
#include <iostream>

//static int cnt = 0;

Component::Component(Actor* a, int order)
    : mOwner(a)
    , mUpdateOrder(order)
{
	// アクターに持たせる
    mOwner->AddComponent(this);
}

Component::~Component()
{
    mOwner->RemoveComponent(this);
}

void Component::Update(float deltaTime)
{
}

Vector3 Component::GetPosition() const
{
    return mOwner->GetPosition();
}
