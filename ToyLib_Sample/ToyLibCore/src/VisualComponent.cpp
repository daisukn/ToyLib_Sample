#include "VisualComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "LightingManager.h"

VisualComponent::VisualComponent(Actor* owner, int drawOrder, VisualLayer layer)
: Component(owner)
, mTexture(nullptr)
, mIsVisible(true)
, mIsBlendAdd(false)
, mLayer(layer)
, mType(VisualType::NoAssigned)
, mDrawOrder(drawOrder)
, mEnableShadow(false)
{
    mOwnerActor->GetApp()->GetRenderer()->AddVisualComp(this);
    mLightingManager = mOwnerActor->GetApp()->GetRenderer()->GetLightingManager();
    
}

VisualComponent::~VisualComponent()
{
    mOwnerActor->GetApp()->GetRenderer()->RemoveVisualComp(this);
}
