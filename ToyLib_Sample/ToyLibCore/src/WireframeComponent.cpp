#include "WireframeComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


WireframeComponent::WireframeComponent(Actor* a)
: Component(a)
{
    mOwnerActor->GetApp()->GetRenderer()->AddWireframeComp(this);
}

WireframeComponent::~WireframeComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveWireframeComp(this);
}
