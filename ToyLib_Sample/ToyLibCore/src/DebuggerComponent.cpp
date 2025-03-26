#include "DebuggerComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"


DebuggerComponent::DebuggerComponent(Actor* a)
: Component(a)
{
    mOwner->GetApp()->GetRenderer()->AddDebuggerComp(this);
}

DebuggerComponent::~DebuggerComponent()
{
    mOwner->GetApp()->GetRenderer()->RemoveDebuggerComp(this);
}
