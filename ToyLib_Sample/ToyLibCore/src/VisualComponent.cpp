#include "VisualComponent.h"
#include "Actor.h"
#include "Application.h"
#include "Renderer.h"
#include "LightingManager.h"

VisualComponent::VisualComponent(Actor* owner, int drawOrder, VisualLayer layer)
: Component(owner)
, mTexture(nullptr)
, mIsVisible(false)
, mIsBlendAdd(false)
, mLayer(layer)
, mType(VisualType::NoAssigned)
, mDrawOrder(drawOrder)
{
    mLightingManager = mOwnerActor->GetApp()->GetRenderer()->GetLightingManager();
}
