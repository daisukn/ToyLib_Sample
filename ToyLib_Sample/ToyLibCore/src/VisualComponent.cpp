#include "VisualComponent.h"

VisualComponent::VisualComponent(Actor* owner, int drawOrder, VisualLayer layer)
: Component(owner)
, mTexture(nullptr)
, mIsVisible(false)
, mIsBlendAdd(false)
, mLayer(layer)
, mType(VisualType::NoAssigned)
, mDrawOrder(0)
{
}
