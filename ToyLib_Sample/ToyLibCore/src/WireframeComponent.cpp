#include "DebuggerComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


DebuggerComponent::DebuggerComponent(Actor* a)
: Component(a)
{
    mOwnerActor->GetApp()->GetRenderer()->AddDebuggerComp(this);
}

DebuggerComponent::~DebuggerComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveDebuggerComp(this);
}
