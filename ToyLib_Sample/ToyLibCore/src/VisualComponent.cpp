#include "VisualComponent.h"

VisualComponent::VisualComponent(Actor* owner, VisualLayer layer)
    : Component(owner)
    , mTexture(nullptr)
    , mIsVisible(true)
    , mIsBlendAdd(false)
    , mLayer(layer)
    , mDrawOrder(0)
{
}
