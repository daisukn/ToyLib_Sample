#pragma once

#include "Component.h"


class VisualComponent : public Component
{
public:
    VisualComponent(class Actor* owner);
    virtual ~VisualComponent() {}

    virtual void Draw(class Shader* shader) = 0;

    void SetTexture(class Texture* tex) { mTexture = tex; }
    class Texture* GetTexture() const { return mTexture; }

    void SetVisible(bool v) { mIsVisible = v; }
    bool IsVisible() const { return mIsVisible; }

    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool IsBlendAdd() const { return mIsBlendAdd; }

protected:
    class Texture* mTexture;
    bool mIsVisible;
    bool mIsBlendAdd;
};

