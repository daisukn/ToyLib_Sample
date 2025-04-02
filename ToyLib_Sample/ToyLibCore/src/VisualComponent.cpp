// VisualComponent.cpp
#include "VisualComponent.h"

VisualComponent::VisualComponent(Actor* owner)
    : Component(owner)
    , mTexture(nullptr)
    , mIsVisible(true)
    , mIsBlendAdd(false)
{
}
