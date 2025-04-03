#pragma once

#include "Component.h"
#include "Renderer.h"



enum class VisualType
{
    NoAssigned,
    Sprite,
    Billboard,
    Particle
};

class VisualComponent : public Component
{
public:
    VisualComponent(class Actor* owner, VisualLayer layer = VisualLayer::Effect3D);
    virtual ~VisualComponent() {}

    virtual void Draw(class Shader* shader) = 0;

    virtual void SetTexture(class Texture* tex) { mTexture = tex; }
    class Texture* GetTexture() const { return mTexture; }

    void SetVisible(bool v) { mIsVisible = v; }
    bool IsVisible() const { return mIsVisible; }

    void SetBlendAdd(bool b) { mIsBlendAdd = b; }
    bool IsBlendAdd() const { return mIsBlendAdd; }

    void SetLayer(VisualLayer layer) { mLayer = layer; }
    VisualLayer GetLayer() const { return mLayer; }
    
    VisualType GetVisualType() const { return mType; }

    int GetDrawOrder() const { return mDrawOrder; }
    void SetDrawOrder(int order) { mDrawOrder = order; }

protected:
    class Texture* mTexture;
    bool mIsVisible;
    bool mIsBlendAdd;
    VisualLayer mLayer;
    VisualType mType;
    int mDrawOrder;
};
